#include "mcp4725.h"

void setup_timer1_4khz() {
    TCCR1A = 0;
    TCCR1B = 0;
    TCCR1B |= 0x08;
    OCR1A = 499;
    TIMSK1 |= 0x02;
}

void start_timer1() {
    TCCR1B |= 0x02;
}

void stop_timer1() {
    TCCR1B &= ~0x07;
}

void write_mcp4725(uint16_t data) {
    write_i2c(MCP4725_ADDR, (uint8_t[]){0x00 | ((data >> 8) & 0x0F), data & 0xFF}, 2);
}
