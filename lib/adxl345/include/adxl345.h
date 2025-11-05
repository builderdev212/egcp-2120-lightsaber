#ifndef ADXL345_H
#define ADXL345_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <atmega328p_i2c.h>

/************************
 * I2C Status Constants *
 ************************/

#define INT_PORT PORTD
#define INT_PORT_IO_CONF DDRD
#define ADXL345_INT_PIN 0x04
#define ADXL345_ADDR 0x53
#define ADXL345_ID 0xE5

/****************
 * Register Map *
 ****************/

#define ADXL345_ID_ADDR 0x00
#define ADXL345_ACT_THRESH 0x24
#define ADXL345_ACT_CTRL 0x27
#define ADXL345_BR 0x2C
#define ADXL345_POWER_CTL 0x2D
#define ADXL345_INT_EN 0x2E
#define ADXL345_INT_MAP 0x2F
#define ADXL345_DATA_FORMAT 0x31
#define ADXL345_DATA_START 0x32

/***************************
 * ADXL345 MACRO Functions *
 ***************************/

#define CONCAT_CAST_2xU8_TO_S16(val1, val2) (int16_t)(((uint16_t)val1 << 8) | (uint16_t)val2)

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
 * @brief Setup INT0 and setup activity interrupts from the ADXL345.
 */
void setup_adxl345_activity_interrupt();

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
