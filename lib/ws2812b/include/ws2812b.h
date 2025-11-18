#ifndef WS2812B_H
#define WS2812B_H

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <avr/interrupt.h>

/*************************
 * RGB command Constants *
 *************************/
#define HIGH 0x01
#define LOW 0x00
#define RESET 0x02
#define NOP() asm volatile("nop")

/*********************************
 * I2C User Function Definitions *
 *********************************/

#ifdef __cplusplus
extern "C" {
#endif

void SendArrayRGB(unsigned char R[], unsigned char G[], unsigned char B[], unsigned int arraySize);

#ifdef __cplusplus
}
#endif

#endif
