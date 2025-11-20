#ifndef WS2812E_H
#define WS2812E_H

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <avr/interrupt.h>

/*********************************
 * LED User Function Definitions *
 *********************************/

#ifdef __cplusplus
extern "C" {
#endif

void setup_ws2812e();

void SendArrayRGB(unsigned char R[], unsigned char G[], unsigned char B[], unsigned int arraySize);

#ifdef __cplusplus
}
#endif

#endif
