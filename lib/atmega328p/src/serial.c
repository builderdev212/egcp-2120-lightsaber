#include "atmega328p/serial.h"

void uart_init(unsigned long baud_rate) {
    // Set baud rate
    unsigned long ubrr = (F_CPU/(16*baud_rate))-1;
    UBRR0H = ((ubrr >> 8) & 0xFF);
    UBRR0L = (ubrr & 0xFF);
    // Enable RX and TX
    UCSR0B = 0x18;
    // 8 data bits, no parity, 1 stop bit
    UCSR0C = 0x06;
}


void uart_print(const char *s) {
    while (*s) {
        while (!(UCSR0A & 0x20));
        UDR0 = *s++;
    }
}