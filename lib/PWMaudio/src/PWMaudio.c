#include "PWMaudio.h"

// ---- Slash (swing) effect globals & tunables ----
static volatile uint8_t  slash_active = 0;
static volatile uint32_t slash_pos = 0;   // sample index
static volatile uint32_t slash_len = 0;   // length in samples

// Tunables (change these for different sounding slashes)
#define SLASH_BASE_FREQ   300   // starting frequency (Hz)
#define SLASH_END_FREQ   1500   // ending frequency (Hz)
#define SLASH_TONE_GAIN    60   // percent (0..100) of tone in mix
#define SLASH_NOISE_GAIN   80   // percent (0..100) of noise in mix

// ---- call this to trigger a slash for `ms` milliseconds ----
void trigger_slash(uint16_t ms) {
    if (ms == 0) ms = 100; // default 100 ms if caller passes 0
    uint32_t len = ((uint32_t)ms * SAMPLE_RATE) / 1000U;
    if (len == 0) len = 1;

    slash_len = len;
    slash_pos = 0;
    slash_active = 1;
}


static uint32_t freq_to_inc(uint16_t freq_hz) {
    return (uint32_t)((((uint64_t)freq_hz) << 32) / SAMPLE_RATE);
}

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

static volatile uint32_t phase1 = 0;
static volatile uint32_t phase2 = 0;
static volatile uint32_t vib_phase = 0;
static uint32_t inc1 = 0;
static uint32_t inc2 = 0;
static uint32_t vib_inc = 0;
static uint32_t lfsr = 0xACE1u;

static inline uint8_t fast_sine_u8(uint32_t phase) {
    uint8_t idx = (uint8_t)(phase >> 24); // top 8 bits as index
    return pgm_read_byte(&sine_table256[idx]);
}

ISR(TIMER2_COMPA_vect) {
        // ----- Slash priority: if active, run slash routine and return -----
    if (slash_active) {
        uint32_t t = slash_pos;
        uint32_t len = (slash_len ? slash_len : 1);

        // Linear sweep frequency from SLASH_BASE_FREQ -> SLASH_END_FREQ
        uint32_t curf = SLASH_BASE_FREQ + ((uint32_t)(SLASH_END_FREQ - SLASH_BASE_FREQ) * t) / len;
        uint32_t cur_inc = freq_to_inc((uint16_t)curf);

        // advance a phase for the slash tone
        phase1 += cur_inc;
        uint8_t tone = fast_sine_u8(phase1); // 0..255

        // cheap noise
        lfsr = (lfsr >> 1) ^ (-(lfsr & 1u) & 0xB400u);
        uint8_t noise = (uint8_t)(lfsr & 0xFF);

        // falling envelope: env = (len - t) / len  scaled to 0..255
        uint16_t env = (uint16_t)(((len - t) * 255UL) / len); // 0..255

        // mix tone and noise with tunable gains, then apply envelope
        int16_t mix = 0;
        mix += ((int16_t)tone - 128) * SLASH_TONE_GAIN / 100;
        mix += ((int16_t)noise - 128) * SLASH_NOISE_GAIN / 100;

        // apply envelope (integer)
        mix = (mix * (int16_t)env) / 255;

        int16_t out = mix + 128;

        if (out < 0) out = 0;
        if (out > 255) out = 255;

        OCR1A = (uint8_t)out;

        // advance and finish
        slash_pos++;
        if (slash_pos >= slash_len) {
            slash_active = 0;
            slash_pos = 0;
            slash_len = 0;
        }
        return; // skip regular hum while slash is playing
    }
    
    // vibrato phase
    vib_phase += vib_inc;
    uint8_t vib_val = fast_sine_u8(vib_phase);
    int8_t vib_offset = (int8_t)((int)vib_val - 128) >> 5;

    uint32_t local_inc1 = inc1 + (((int32_t)inc1 * vib_offset) / 128);
    uint32_t local_inc2 = inc2 + (((int32_t)inc2 * vib_offset) / 128);

    phase1 += local_inc1;
    phase2 += local_inc2;

    uint8_t s1 = fast_sine_u8(phase1);
    uint8_t s2 = fast_sine_u8(phase2);

    // LFSR noise
    lfsr = (lfsr >> 1) ^ (-(lfsr & 1u) & 0xB400u);
    uint8_t noise = (uint8_t)(lfsr & 0xFF);

    int16_t mix = 0;
    mix += ((int16_t)s1 - 128) * 55 / 100;
    mix += ((int16_t)s2 - 128) * 55 / 100;
    mix += ((int16_t)noise - 128) * 25 / 100;

    mix += 128;
    if (mix < 0)
        mix = 0;
    if (mix > 255)
        mix = 255;

    OCR1A = (uint8_t)mix;
}

void setupPWM() {
    PWM_PORT_IO_CONF |= PWM_PIN;
    TCCR1A = (1 << COM1A1) | (1 << WGM10);
    TCCR1B = (1 << WGM12) | (1 << CS10);
    OCR1A = 128;

    TCCR2A = (1 << WGM21);
    TCCR2B = (1 << CS21) | (1 << CS20); // prescaler 32

    uint32_t ticks = (F_CPU / (32UL * SAMPLE_RATE));
    if (ticks == 0)
        ticks = 1;
    if (ticks > 255)
        ticks = 255;
    OCR2A = (uint8_t)(ticks - 1);

    uint16_t f1 = 60;      // base hum
    uint16_t f2 = 63;      // detuned
    uint16_t vib_freq = 4; // vibrato frequency (Hz)
    inc1 = freq_to_inc(f1);
    inc2 = freq_to_inc(f2);
    vib_inc = freq_to_inc(vib_freq);

    TIMSK2 = (1 << OCIE2A);
}
