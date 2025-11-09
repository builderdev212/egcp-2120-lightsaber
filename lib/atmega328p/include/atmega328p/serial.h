#ifndef ATMEGA328P_SERIAL_H
#define ATMEGA328P_SERIAL_H

#include <avr/io.h>
#include <stdint.h>

/************************************
 * Serial User Function Definitions *
 ************************************/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Setup the UART hardware.
 * 
 * @param baud_rate serial rate to communicate at.
 */
void uart_init(unsigned long baud_rate);

/**
 * @brief Output a string over serial.
 * 
 * @param s c-string to print.
 */
void uart_print(const char *s);

#ifdef __cplusplus
}
#endif

#endif
