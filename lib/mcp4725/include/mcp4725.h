#ifndef MCP4725_H
#define MCP4725_H

#include <avr/io.h>
#include <stdint.h>
#include <atmega328p/i2c.h>
#include <atmega328p/serial.h>

/*********************
 * MCP4725 Constants *
 *********************/

#define MCP4725_ADDR 0xC0

/*************************************
 * MCP4725 User Function Definitions *
 *************************************/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Setup Timer 1 to run at 4KHz to match WAV files.
 */
void setup_timer1_4khz();

/**
 * @brief Start Timer1.
 */
void start_timer1();

/**
 * @brief Stop Timer1.
 */
void stop_timer1();

/**
 * @brief Output a given value to the DAC.
 * 
 * @param data 12-bit value to be passed straight to the DAC.
 */
void write_mcp4725(uint16_t data);

#ifdef __cplusplus
}
#endif

#endif
