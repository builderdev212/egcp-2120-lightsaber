#ifndef ADXL345_H
#define ADXL345_H

#include <avr/io.h>
#include <atmega328p_i2c.h>

/************************
 * I2C Status Constants *
 ************************/

#define INT_PORT PORTD
#define INT_PORT_IO_CONF DDRD
#define ADXL345_INT_PIN 0x04
#define ADXL345_ADDR 0x53
#define ADXL345_ID 0xE5

/*********************************
 * I2C User Function Definitions *
 *********************************/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Data structure to store acceleration data.
 * 
 * @param x acceleration in the x axis.
 * @param y acceleration in the y axis.
 * @param z acceleration in the z axis.
 */
struct acceleration {
    int16_t x;
    int16_t y;
    int16_t z;
};

/**
 * @brief Check if the ADXL345 is on the I2C bus.
 * 
 * @return 1 if the device is found, otherwise 0.
 */
uint8_t is_adxl345_present();

/**
 * @brief Put the ADXL345 into measure mode and ensure it is out of sleep.
 */
void enable_adxl345();

/**
 * @brief Get the current acceleration measured by the accelerometer.
 * 
 * @return acceleration struct.
 */
struct acceleration get_current_acceleration();

#ifdef __cplusplus
}
#endif

#endif
