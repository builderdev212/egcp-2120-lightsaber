#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include <stdint.h>
#include <atmega328p/i2c.h>
#include <atmega328p/serial.h>
#include <mpu6050.h>
#include <PWMaudio.h>
#include <ws2812e.h>

// Accelerometer Interupt
volatile uint8_t accelerometer_flag = 0;
ISR(INT0_vect) {
    accelerometer_flag = 1;
}

// Use a static sine table instead of the sin() function
static const uint8_t sine_table256[256] PROGMEM = {
    128, 131, 134, 137, 140, 143, 146, 149, 152, 155, 158, 161, 164, 167, 170, 173,
    176, 179, 182, 185, 188, 191, 194, 197, 200, 203, 206, 209, 212, 215, 218, 221,
    224, 227, 230, 233, 236, 239, 242, 244, 247, 250, 252, 255, 255, 255, 255, 255,
    255, 255, 255, 252, 250, 247, 244, 242, 239, 236, 233, 230, 227, 224, 221, 218,
    215, 212, 209, 206, 203, 200, 197, 194, 191, 188, 185, 182, 179, 176, 173, 170,
    167, 164, 161, 158, 155, 152, 149, 146, 143, 140, 137, 134, 131, 128, 125, 122,
    119, 116, 113, 110, 107, 104, 101, 98, 95, 92, 89, 86, 83, 80, 77, 74,
    71, 68, 65, 62, 59, 56, 53, 51, 48, 45, 43, 40, 38, 35, 33, 31,
    28, 26, 24, 22, 20, 18, 16, 15, 13, 12, 11, 10, 9, 8, 8, 7,
    7, 7, 7, 7, 8, 8, 9, 10, 11, 12, 13, 15, 16, 18, 20, 22,
    24, 26, 28, 31, 33, 35, 38, 40, 43, 45, 48, 51, 53, 56, 59, 62,
    65, 68, 71, 74, 77, 80, 83, 86, 89, 92, 95, 98, 101, 104, 107, 110,
    113, 116, 119, 122, 125, 128
};

static inline uint8_t fast_sine_u8(uint32_t phase) {
    uint8_t idx = (uint8_t)(phase >> 24);
    return pgm_read_byte(&sine_table256[idx]);
}

// Hum output variables
static volatile uint8_t  slash_active = 0;
static volatile uint32_t slash_pos = 0;
static volatile uint32_t slash_len = 0;
static volatile uint32_t phase1 = 0;
static volatile uint32_t phase2 = 0;
static volatile uint32_t vib_phase = 0;
static uint32_t inc1 = 0;
static uint32_t inc2 = 0;
static uint32_t vib_inc = 0;
static uint32_t lfsr = 0xACE1u;

// helper function to turn a frequency into an incremented value
static uint32_t freq_to_inc(uint16_t freq_hz) {
    return (uint32_t)((((uint64_t)freq_hz) << 32) / SAMPLE_RATE);
}

static inline uint8_t lfsr_next_byte() {
    lfsr = (uint16_t)((lfsr >> 1) ^ (-(int)(lfsr & 1u) & 0xB400u));
    return (uint8_t)(lfsr & 0xFFu);
}

static inline void slash(void) {
    if (!slash_active) return;
    if (slash_len == 0) {
        slash_active = 0;
        slash_pos = 0;
        return;
    }

    uint32_t t   = slash_pos;
    uint32_t len = slash_len;
    // current frequency
    uint32_t curf = SLASH_BASE_FREQ + (((uint32_t)SLASH_END_FREQ - (uint32_t)SLASH_BASE_FREQ) * t) / len;
    // convert into increment
    uint16_t cur_inc = freq_to_inc((uint16_t)curf);
    // step forward and find tone
    phase1 += cur_inc;
    uint8_t tone = fast_sine_u8(phase1);
    // update noise
    uint8_t noise = lfsr_next_byte();
    // setup min/max
    uint32_t env = (uint32_t)(((len - t) * 255UL + len/2) / len);
    // mix noise into current signal
    int32_t mix = ((int32_t)tone - 128) * (int32_t)SLASH_TONE_GAIN / 100 + ((int32_t)noise - 128) * (int32_t)SLASH_NOISE_GAIN / 100;
    // put mix into min/max environment
    mix = (int32_t)((mix * (int32_t)env + 127) / 255);
    // move back to output range
    int32_t out = mix + 128;
    setInc((uint8_t)out);

    // advance position/disable
    slash_pos++;
    if (slash_pos >= slash_len) {
        slash_active = 0;
        slash_pos = 0;
        slash_len = 0;
    }
}

// Constants for the mixing weights (make the meaning explicit)
#define MIX_WEIGHT_S1 55   // percent weight for sine1
#define MIX_WEIGHT_S2 55   // percent weight for sine2
#define MIX_WEIGHT_NOISE 25 // percent weight for noise
// Note: MIX_WEIGHT_S1 + MIX_WEIGHT_S2 + MIX_WEIGHT_NOISE = 135%
// The code subtracts 128 from each source before weighting, then adds 128 back.

static inline void hum(void) {
    // 1) Advance the vibrato phase
    vib_phase += vib_inc;

    // 2) Compute a small signed vibrato offset from the vibrato waveform.
    // fast_sine_u8 returns 0..255, so subtract 128 to center at 0.
    int16_t vib_sin_centered = (int16_t)fast_sine_u8(vib_phase) - 128;
    // Shift to create a small 8-bit signed offset (divide by 32)
    int8_t vib_offset = (int8_t)(vib_sin_centered >> 5);

    // 3) Apply a small proportional modulation to each phase increment (inc1/inc2).
    //    local_inc = base_inc + base_inc * vib_offset / 128
    // Use 32-bit math to avoid overflow on intermediate multiplication.
    int32_t local_inc1 = (int32_t)inc1 + ((int32_t)inc1 * (int32_t)vib_offset) / 128;
    int32_t local_inc2 = (int32_t)inc2 + ((int32_t)inc2 * (int32_t)vib_offset) / 128;

    // 4) Advance the two oscillator phases with the modulated increments.
    phase1 += (uint32_t)local_inc1;
    phase2 += (uint32_t)local_inc2;

    // 5) Sample the two oscillators (sine lookup) as unsigned 0..255 values.
    uint8_t s1 = fast_sine_u8(phase1);
    uint8_t s2 = fast_sine_u8(phase2);

    // 6) Generate a noise sample (0..255)
    uint8_t noise = lfsr_next_byte();

    // 7) Mix the three sources:
    //    - convert each from 0..255 to centered signed (-128..127)
    //    - apply percentage weights
    //    - sum and re-center back to 0..255 by adding 128
    int32_t centered_s1 = (int32_t)s1 - 128;
    int32_t centered_s2 = (int32_t)s2 - 128;
    int32_t centered_noise = (int32_t)noise - 128;

    int32_t mix = (centered_s1 * MIX_WEIGHT_S1) / 100
                + (centered_s2 * MIX_WEIGHT_S2) / 100
                + (centered_noise * MIX_WEIGHT_NOISE) / 100;

    mix += 128;

    // 8) Clamp result to 0..255 in case of overflow, then pass to setInc (which expects a byte).
    if (mix < 0) mix = 0;
    else if (mix > 255) mix = 255;

    setInc((uint8_t)mix);
}

// interrupt for 8khz output
volatile uint8_t sound_flag = 0;
ISR(TIMER2_COMPA_vect) {
    sound_flag = 1;
}

unsigned char black[60] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
unsigned char white[60] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
unsigned char r[60];
unsigned char g[60];
unsigned char b[60];
unsigned char r_off[60];
unsigned char g_off[60];
unsigned char b_off[60];

void setColor(unsigned char nr[], unsigned char ng[], unsigned char nb[]) {
    memcpy(r, nr, sizeof(white));
    memcpy(g, ng, sizeof(white));
    memcpy(b, nb, sizeof(white));
}

void on() {
    for(int i = 1; i <= 60; i ++) {
        SendArrayRGB(r, g, b, i);
        _delay_ms(5);
    }
}

void off() {
    memcpy(r_off, r, sizeof(white));
    memcpy(g_off, g, sizeof(white));
    memcpy(b_off, b, sizeof(white));
    for(int i = 59; i >= 0; i--) {
        r_off[i] = 0;
        g_off[i] = 0;
        b_off[i] = 0;
        SendArrayRGB(r_off, g_off, b_off, 60);
        _delay_ms(5);
    }
}

void colorMode() {
    static uint8_t mode = 0;
    switch (mode) {
        case 1:
            setColor(white, white, black);
            mode++;
            break;
        case 2:
            setColor(black, white, black);
            mode++;
            break;
        case 3:
            setColor(black, black, white);
            mode++;
            break;
        case 4:
            setColor(white, black, white);
            mode++;
            break;
        default:
            setColor(white, black, black);
            mode = 1;
            break;
    }
}

void status() {
    static uint8_t state = 0;
    if (state == 0) {
        on();
        state = 1;
    } else {
        off();
        state = 0;
    }
}

int main() {
    // Setup base hum and initialize audio
    inc1 = freq_to_inc(60);
    inc2 = freq_to_inc(63);
    vib_inc = freq_to_inc(4);
    setupPWM();

    // Initialize hardware
    uart_init(9600);
    start_i2c();
    setup_mpu6050(5, 1);
    DDRD |= (1 << PD3);
    DDRD &= ~(1 << PD4);
    colorMode();

    // enable global interrupts
    sei();

    uint8_t pressed = 0;
    uint16_t held_ms = 0;

    while (1) {
        if (sound_flag == 1) {
            if (slash_active) {
                slash();
            } else {
                hum();
            }
            sound_flag = 0;
        }
        if (accelerometer_flag == 1) {
            clear_mpu6050_int_status();
            slash_len = (1000 * SAMPLE_RATE) / 1000U;
            slash_pos = 0;
            slash_active = 1;
            accelerometer_flag = 0;
        }

        if (((PIND & (1 << PD4)) == 0)) {
            while (((PIND & (1 << PD4)) == 0)) {
                _delay_ms(1);
                held_ms += 1;
            }
            pressed = 1;

            if (held_ms > 500) {
                colorMode();
                uart_print("color");
            } else {
                status();
            }
            held_ms = 0;
        }
    }

    return 0;
}
