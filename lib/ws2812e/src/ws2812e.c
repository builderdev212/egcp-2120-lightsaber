#include "ws2812e.h"

static inline void PortD3(unsigned char value) {
    if(value == 0) {
        PORTD &= ~(1 << PD3);
    } else {
        PORTD |= (1 << PD3);
    }
}

static inline void Logic1() {
    PortD3(1);
    NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP();  // 16xNOP() = 0.8 microsecond delay
    PortD3(0);
    NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); // 9xNOP() = 0.45 microsecond delay
}

static inline void Logic0() {
    PortD3(1);
    NOP(); NOP();  // 8xNOP() = 0.4 microsecond delay
    PortD3(0);
    NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP();  // 17xNOP() = 0.85 microsecond delay
}

static inline void ResetCode() {
    PortD3(0);
    //Delay >= 50 microseconds = 100xNOP()
    NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); // 10xNOP() = 5 microsecond delay
    NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP();
    NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP();
    NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP();
    NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP();
    NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); 
    NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); 
    NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); 
    NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP();
    NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP();
}

static inline void SendLogic(unsigned char code) {
    if(code == HIGH) {
        Logic1();
    } else if(code == LOW) {
        Logic0();
    } else if(code == RESET) {
        ResetCode();
    }
}

static inline void SendByte(unsigned char sendByte) {
    unsigned char bit;

    for(char i = 7; i >= 0; i--) {
        bit = (sendByte >> i) & 0x01; 
        SendLogic(bit);
    }
}

static inline void SendOneRGB(unsigned char R, unsigned char G, unsigned char B) {
    // the standard neopixel LED recieves RGB values in the order B, R, and G
    SendByte(G);
    SendByte(R);
    SendByte(B);
}

void SendArrayRGB(unsigned char R[], unsigned char G[], unsigned char B[], unsigned int arraySize) {
    unsigned char currR; // these are necessary so as not to modify the array
    unsigned char currG;
    unsigned char currB;

    cli(); //Disable interupts
    for(unsigned int i = 0; i < arraySize; i++) { //This only works if the array size is correct
        currR = R[i];
        currG = G[i];
        currB = B[i];
        SendOneRGB(currR, currG, currB);
    }
    SendLogic(RESET);
    sei(); //Enable interupts
}
