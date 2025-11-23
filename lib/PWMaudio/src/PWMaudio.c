#include "PWMaudio.h"

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

    TIMSK2 = (1 << OCIE2A);
}

void setInc(uint8_t out) {
    if (out < 0) out = 0;
    if (out > 255) out = 255;
    OCR1A = out;
}
