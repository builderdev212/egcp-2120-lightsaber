#include "PWMaudio.h"

void setupPWM() {
    PWM_PORT_IO_CONF |= PWM_PIN;
    PWM_PORT &= ~PWM_PIN;
    // Setup PWM
    TCCR1A = 0x20;
    TCCR1B = 0x0B;
    setFrequency(0);
}

void setFrequency(uint16_t freq) {
    if (freq == 0) {
        TCCR1A &= ~(1 << COM1A0);
        PORTB &= ~(1 << PB1);
    } else {
        TCCR1A |= (1 << COM1A0);
        OCR1A = ((F_CPU / (2UL * 64UL * freq) - 1UL) > 0xFFFFUL ? 0xFFFFUL : (F_CPU / (2UL * 64UL * freq) - 1UL));
    }
}