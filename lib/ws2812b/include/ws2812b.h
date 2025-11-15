#ifndef WS2812B_H
#define WS2812B_H

/*************************
 * RGB command Constants *
 *************************/

#define HIGH 0x01
#define LOW 0x00
#define RESET 0x02

#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>

/*********************************
 * I2C User Function Definitions *
 *********************************/

#ifdef __cplusplus
extern "C" {
#endif

void PortD0(unsigned char value) {
    if(value == 0) {
        PORTD &= 0xFE;
    } else {
        PORTD |= 0x01;
    }
}

void Logic1() {
    PortD0(1);
    NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP();  // 16xNOP() = 0.8 microsecond delay
    PortD0(0);
    NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); // 9xNOP() = 0.45 microsecond delay
}

void Logic0() {
    PortD0(1);
    NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); // 8xNOP() = 0.4 microsecond delay
    PortD0(0);
    NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); // 17xNOP() = 0.85 microsecond delay
}

void ResetCode() {
    PortD0(0);
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

void SendLogic(unsigned char code) {
    if(code == HIGH) {
        Logic1();
    } else if(code == LOW) {
        Logic0();
    } else if(code == RESET) {
        ResetCode();
    }
}

void SendByte(unsigned char sendByte) {
    unsigned char bit;

    bit = sendByte >> 7;
    SendLogic(bit);
    bit = (sendByte & 0x40) >> 6;
    SendLogic(bit);
    bit = (sendByte & 0x20) >> 5;
    SendLogic(bit);
    bit = (sendByte & 0x10) >> 4;
    SendLogic(bit);
    bit = (sendByte & 0x08) >> 3;
    SendLogic(bit);
    bit = (sendByte & 0x04) >> 2;
    SendLogic(bit);
    bit = (sendByte & 0x02) >> 1;
    SendLogic(bit);
    bit = (sendByte & 0x01);
    SendLogic(bit);
}

void SendOneRGB(unsigned char R, unsigned char G, unsigned char B) {
    // the standard neopixel LED recieves RGB values in the order B, R, and G
    SendByte(B);
    SendByte(R);
    SendByte(G);
}

void SendArrayRGB(unsigned char R[], unsigned char G[], unsigned char B[], unsigned int arraySize) {
    unsigned char currR; // these are necessary so as not to modify the array
    unsigned char currG;
    unsigned char currB;

    cli(); //Disable interupts
    for(int i = 0; i < arraySize; i++) { //This only works if the array size is correct
        currR = R[i];
        currG = G[i];
        currB = B[i];
        SendOneRGB(currR, currG, currB);
    }
    SendLogic(RESET);
    sei(); //Enable interupts
}

#ifdef __cplusplus
}
#endif

#endif