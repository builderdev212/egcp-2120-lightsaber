#ifndef ATMEGA328P_I2C_H
#define ATMEGA328P_I2C_H

#include <avr/io.h>
#include <stdint.h>

/************************
 * I2C Status Constants *
 ************************/

#define I2C_PORT PORTC
#define I2C_PORT_IO_CONF DDRC
#define I2C_SDA_PIN 0x10 // pin A4
#define I2C_SCL_PIN 0x20 // pin A5

#define I2C_START 0x08 // sent start
#define I2C_REPEATED_START 0x10 // sent restart
#define I2C_SEND_WADDR 0x18 // sent write address
#define I2C_SEND_RADDR 0x40 // sent read address
#define I2C_SEND_DATA 0x28 // sent data
#define I2C_RECV_DATA 0x50 // recieved data
#define I2C_RECV_DATA_END 0x58 // controller propagated nack to end read from target device

/******************************************************************
 * I2C Internal MACROS                                            *
 * used by the precompiler as "copy and paste" code, removes      *
 * unneccesary function calls while cleaning up the overall code. *
 ******************************************************************/

/**
 * @brief Wait for the current stage of the transaction to finish.
 */
#define WAIT_FOR_TRANSMIT() while(!((TWCR & 0x80) >> 7))

/**
 * @brief Check the current STATUS of the I2C hardware.
 * 
 * @param expected_status used to check if the status in hardware matches the expected status.
 */
#define STATUS(expected_status) ((TWSR & 0xF8) == expected_status)

/**
 * @brief Transmit START to begin the I2C transaction.
 */
#define START() \
    TWCR = 0xA4; \
    WAIT_FOR_TRANSMIT(); \
    if (!STATUS(I2C_START)) return -2

#define RESTART() \
    TWCR = 0xA4; \
    WAIT_FOR_TRANSMIT(); \
    if (!STATUS(I2C_REPEATED_START)) return -2

/**
 * @brief SEND data over the I2C bus to the target device.
 * 
 * @param data 8-bit value to be sent to the target.
 * @param expected_status should be either I2C_SEND_ADDR or I2C_SEND_DATA
 */
#define SEND(data, expected_status) \
    TWDR = data; \
    TWCR = 0x84; \
    WAIT_FOR_TRANSMIT(); \
    if (!STATUS(expected_status)) return -1

/**
 * @brief RECIEVE data from the target I2C device.
 * 
 * @param data used to store the recieved data from the target.
 * @param last set to 1 if this is the final byte to be read, 0 otherwise.
 */
#define RECV(data, last) \
    TWCR = 0x84 | ((last) ? 0x00 : 0x40); \
    WAIT_FOR_TRANSMIT(); \
    if (!STATUS((last) ? I2C_RECV_DATA_END : I2C_RECV_DATA)) return -1; \
    data = (uint8_t)TWDR

/**
 * @brief Transmit STOP to end I2C transaction.
 */
#define STOP() \
    TWCR = 0x94; \
    while ((TWCR & 0x10) >> 4)

/*********************************
 * I2C User Function Definitions *
 *********************************/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Setup the I2C hardware.
 */
void start_i2c();

/**
 * @brief I2C write function.
 * 
 * @param dev_addr target device address.
 * @param data pointer to array of data to be sent over i2c.
 * @param length length of the array.
 */
int write_i2c(uint8_t dev_addr, uint8_t *data, uint8_t length);

/**
 * @brief I2C read function.
 * 
 * @param dev_addr target device address.
 * @param data pointer to array to store the data recieved.
 * @param length of the data to be recieved.
 */
int read_i2c(uint8_t dev_addr, uint8_t *data, uint8_t length);

/**
 * @brief Set address and read function.
 * 
 * @param dev_addr target device address.
 * @param starting_reg_addr starting address of the EEPROM to read from.
 * @param data pointer to array to store the data recieved.
 * @param length of the data to be recieved.
 */
int addr_read_i2c(uint8_t dev_addr, uint8_t starting_reg_addr, uint8_t *data, uint8_t length);

#ifdef __cplusplus
}
#endif

#endif
