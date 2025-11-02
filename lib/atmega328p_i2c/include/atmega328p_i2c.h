#ifndef ATMEGA328P_I2C
#define ATMEGA328P_I2C

#include <avr/io.h>

/************************
 * I2C Status Constants *
 ************************/

#define I2C_PORT PORTC
#define I2C_PORT_IO_CONF DDRC
#define I2C_SDA_PIN 0x10 // pin 4
#define I2C_SCL_PIN 0x20 // pin 5

#define I2C_START 0x08 // sent start
#define I2C_SEND_WADDR 0x18 // sent write address
#define I2C_SEND_RADDR 0x40 // sent read address
#define I2C_SEND_DATA 0x28 // sent data
#define I2C_RECV_DATA 0x50 // recieved data
#define I2C_RECV_DATA_END 0x58 // controller propagated nack to end read from target device

/*********************************
 * I2C User Function Definitions *
 *********************************/

#ifdef __cplusplus
extern "C" {
#endif

void start_i2c();
void write_i2c(uint8_t dev_addr, uint8_t *data, uint8_t length);
void read_i2c(uint8_t dev_addr, uint8_t *data, uint8_t length);

#ifdef __cplusplus
}
#endif

#endif
