#include <avr/io.h>
#include <avr/interrupt.h>
#include <atmega328p_i2c.h>
#include <adxl345.h>
#include <mcp4725.h>
#include <ws2812b.h>
#define F_CPU 16000000UL
#include <util/delay.h>

// Accelerometer Interupt
//uint8_t a_flag = 0;
volatile uint8_t a_flag = 0; //HEY RYAN, I CHANGED THIS... MAYBE ITS HELPFUL?
ISR(INT0_vect) {
    a_flag = 1;
}

int main() {
    DDRD |= (1 << PD3); //Pin D3 is output
    
unsigned char R[60] = {
255,255,255,255,255,255,255,255,255,255,
255,230,204,178,153,128,102,77,51,26,
0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,
0,25,51,76,102,128,153,179,204,230,
255,255,255,255,255,255,255,255,255,255
};

unsigned char G[60] = {
0,25,51,77,102,128,153,178,204,229,
255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,
255,230,204,178,153,128,102,76,51,25,
0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0
};

unsigned char B[60] = {
0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,
0,25,51,76,102,128,153,179,204,229,
255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,
255,230,204,179,153,128,102,77,51,26
};


    unsigned char black[60] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    unsigned char white[60] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    unsigned char one[1] = {0xFF};
    unsigned char zero[1] = {0};
    while (1) {

        SendArrayRGB(R, G, B, 60);
        _delay_ms(500);

    }
}

