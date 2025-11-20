#include "ws2812e.h"

static inline void delay_nops(int count) {
    for (int i = 0; i < count; i++) {
        __asm__ __volatile__("nop");
    }
}

static inline void PortD3(unsigned char value) {
    if(value == 0) {
        PORTD &= ~(1 << PD3);
    } else {
        PORTD |= (1 << PD3);
    }
}

static inline void Logic1() {
    PortD3(1);
    delay_nops(13); // 0.8us delay
    PortD3(0);
    delay_nops(7); // 0.45us delay
}

static inline void Logic0() {
    PortD3(1);
    delay_nops(2); // 0.4 us delay
    PortD3(0);
    delay_nops(15); // 0.85us delay
}

static inline void ResetCode() {
    PortD3(0);
    _delay_ms(100);
}

static inline void SendByte(unsigned char sendByte) {
    uint8_t currByte = sendByte;
    unsigned char bit;

    for(char i = 7; i >= 0; i--) {
        bit = (currByte >> i) & 0x01; 
        if (bit == 1) {
            Logic1();
        } else if (bit == 0) {
            Logic0();
        }
    }
}

void setup_ws2812e() {
    DDRD |= (1 << PD3);
}

void SendArrayRGB(unsigned char R[], unsigned char G[], unsigned char B[], unsigned int arraySize) {
    cli();
    for(unsigned int i = 0; i < arraySize; i++) {
        SendByte(G[i]);
        SendByte(R[i]);
        SendByte(B[i]);
    }
    ResetCode();
    sei();
}
