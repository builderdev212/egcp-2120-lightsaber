#include "atmega328p_i2c.h"

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
    if (!STATUS(I2C_START)) return

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
    if (!STATUS(expected_status)) return

/**
 * @brief RECIEVE data from the target I2C device.
 * 
 * @param data used to store the recieved data from the target.
 * @param last set to 1 if this is the final byte to be read, 0 otherwise.
 */
#define RECV(data, last) \
    TWCR = 0x84 | ((last == 1) ? 0x00 : 0x40); \
    WAIT_FOR_TRANSMIT(); \
    if (!STATUS((last == 1) ? I2C_RECV_DATA_END : I2C_RECV_DATA)) return; \
    data = (uint8_t)TWDR

/**
 * @brief Transmit STOP to end I2C transaction.
 */
#define STOP() TWCR = 0x94

/**********************
 * I2C User Functions *
 **********************/

void start_i2c() {
    // Turn the clock to the i2c module on.
    PRR &= 0x7F;
    // enable internal pullups and set the I2C pins to input
    I2C_PORT |= I2C_SDA_PIN | I2C_SCL_PIN;
    I2C_PORT_IO_CONF &= ~(I2C_SDA_PIN | I2C_SCL_PIN);
    // Setup the I2C timing register
    TWCR = 0x00; // set the prescaler to 1
    TWBR = 72; // see section 21.5.2 for this calculation, should set SCL speed to 100KHz based on 16MHz input clk
    // Start the I2C controller
    TWCR = 0x04; 
}

/**
 * @brief I2C write function.
 * 
 * @param dev_addr target device address.
 * @param data pointer to array of data to be sent over i2c.
 * @param length length of the array.
 */
void write_i2c(uint8_t dev_addr, uint8_t *data, uint8_t length) {
    START();
    SEND(dev_addr, I2C_SEND_WADDR);
    for (uint8_t i = 0; i < length; i++) {
        SEND(*(data + i), I2C_SEND_DATA);
    }
    STOP();
}

/**
 * @breif I2C read function.
 * 
 * @param dev_addr target device address.
 * @param data pointer to array to store the data recieved.
 * @param length of the data to be recieved.
 */
void read_i2c(uint8_t dev_addr, uint8_t *data, uint8_t length) {
    START();
    SEND(dev_addr | 0x01, I2C_SEND_RADDR);
    for (uint8_t i = 0; i < length; i++) {
        RECV(*(data + i), ((i == length-1) ? 1 : 0));
    }
    STOP();
}
