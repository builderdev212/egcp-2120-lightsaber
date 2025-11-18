#ifndef WS2812B_H
#define WS2812B_H

/*************************
 * RGB command Constants *
 *************************/
#define HIGH 0x01
#define LOW 0x00
#define RESET 0x02
#define NOP() asm volatile("nop")

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <avr/interrupt.h>

/*********************************
 * I2C User Function Definitions *
 *********************************/

#ifdef __cplusplus
extern "C" {
#endif

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

static inline void SendArrayRGB(unsigned char R[], unsigned char G[], unsigned char B[], unsigned int arraySize) {
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

#ifdef __cplusplus
}
#endif

#endif

/* Assumes F_CPU == 16000000UL and PD3 used as output pin */



// #include <util/delay.h>
// #include <avr/io.h>
// #include <avr/interrupt.h>
// #include <stdint.h>

// static inline void ws2812e_init(void) {
//     DDRD |= (1 << PD3);   // PD3 output
//     PORTD &= ~(1 << PD3);
// }

// /* Reset / latch: WS2812E margin, use 300us */
// static inline void ws2812e_reset(void) {
//     PORTD &= ~(1 << PD3);
//     _delay_us(300);
// }

// /* Tight, inline byte-send for AVR @16MHz.
//  * Sends one byte (MSB first). Caller must have disabled interrupts.
//  */
// static inline void ws2812e_sendbyte_avr16(uint8_t b) {
//     volatile uint8_t *port = &PORTD;
//     uint8_t pinmask = (1 << PD3);
//     uint8_t hi = *port | pinmask;
//     uint8_t lo = *port & ~pinmask;

//     for (int8_t i = 7; i >= 0; --i) {
//         if (b & (1 << i)) {
//             /* '1' bit: long high (~0.8us) */
//             *port = hi;
//             asm volatile(
//                 "nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t"
//                 "nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t"
//                 "nop\n\t""nop\n\t""nop\n\t""nop\n\t"
//             );
//             *port = lo;
//             asm volatile("nop\n\t""nop\n\t");
//         } else {
//             /* '0' bit: ultra short high (~<300ns), long low */
//             *port = hi;
//             /* just one or two NOPs */
//             asm volatile("nop\n\t""nop\n\t");
//             *port = lo;
//             /* long low to make full 1.25us period */
//             asm volatile(
//                 "nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t"
//                 "nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t"
//                 "nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t"
//                 "nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t"
//                 "nop\n\t"
//             );
//         }
//     }
// }


// /* Send a pixel in GRB order */
// static inline void ws2812e_send_pixel_avr16(uint8_t r, uint8_t g, uint8_t b) {
//     /* caller must have interrupts disabled */
//     ws2812e_sendbyte_avr16(g);
//     ws2812e_sendbyte_avr16(r);
//     ws2812e_sendbyte_avr16(b);
// }

// /* Send arrays: R[], G[], B[], count */
// static inline void ws2812e_send_array_avr16(uint8_t R[], uint8_t G[], uint8_t B[], uint16_t count) {
//     cli(); /* disable interrupts for timing */
//     for (uint16_t i = 0; i < count; ++i) {
//         ws2812e_send_pixel_avr16(R[i], G[i], B[i]);
//     }
//     ws2812e_reset();
//     sei();
// }
