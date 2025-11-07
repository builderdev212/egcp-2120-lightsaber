#ifndef MCP4725_H
#define MCP4725_H

#include <avr/io.h>
#include <stdint.h>
#include <atmega328p_i2c.h>

/*********************
 * MCP4725 Constants *
 *********************/

#define MCP4725_ADDR 0x60

/*************************************
 * MCP4725 User Function Definitions *
 *************************************/

#ifdef __cplusplus
extern "C" {
#endif

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
