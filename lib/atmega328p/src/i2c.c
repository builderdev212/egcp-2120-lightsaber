#include "atmega328p/i2c.h"

void start_i2c() {
    // turn the clock to the i2c module on
    PRR &= 0x7F;
    // enable internal pullups and set the I2C pins to input
    I2C_PORT |= I2C_SDA_PIN | I2C_SCL_PIN;
    // I2C_PORT_IO_CONF &= ~(I2C_SDA_PIN | I2C_SCL_PIN);
    // Setup the I2C timing register
    TWSR = 0x00; // set the prescaler to 1
    TWBR = 12; // see section 21.5.2 for this calculation, should set SCL speed to 400KHz based on 16MHz input clk
    // Start the I2C controller
    TWCR = 0x04;
}

int write_i2c(uint8_t dev_addr, uint8_t *data, uint8_t length) {
    START();
    SEND(dev_addr, I2C_SEND_WADDR);
    for (uint8_t i = 0; i < length; i++) {
        SEND(*(data + i), I2C_SEND_DATA);
    }
    STOP();
    return 1;
}

int read_i2c(uint8_t dev_addr, uint8_t *data, uint8_t length) {
    START();
    SEND(dev_addr | 0x01, I2C_SEND_RADDR);
    for (uint8_t i = 0; i < length; i++) {
        RECV(*(data + i), ((i == length-1) ? 1 : 0));
    }
    STOP();
    return 1;
}

int addr_read_i2c(uint8_t dev_addr, uint8_t starting_reg_addr, uint8_t *data, uint8_t length) {
    START();
    SEND(dev_addr, I2C_SEND_WADDR);
    SEND(starting_reg_addr, I2C_SEND_DATA);
    RESTART();
    SEND(dev_addr | 0x01, I2C_SEND_RADDR);
    for (uint8_t i = 0; i < length; i++) {
        RECV(*(data + i), ((i == length-1) ? 1 : 0));
    }
    STOP();
    return 1;
}
