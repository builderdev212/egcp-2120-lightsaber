#ifndef MPU6050_H
#define MPU6050_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>
#include <atmega328p/i2c.h>

/*********************
 * MPU6050 Constants *
 *********************/

#define INT_PORT PORTD
#define INT_PORT_IO_CONF DDRD
#define MPU6050_INT_PIN 0x04
#define MPU6050_ADDR 0xD0

/****************
 * Register Map *
 ****************/

#define MPU6050_SMPLRT_DIV 0x19
#define MPU6050_CONFIG 0x1A
#define MPU6050_ACCEL_CONFIG 0x1C
#define MPU6050_MOT_THR 0x1F
#define MPU6050_MOT_DUR 0x20
#define MPU6050_INT_PIN_CONFIG 0x37
#define MPU6050_INT_EN 0x38
#define MPU6050_INT_STATUS 0x3A
#define MPU6050_MOT_DETECT_CTRL 0x69
#define MPU6050_PWR_MGMT_1 0x6B

/*************************************
 * MPU6050 User Function Definitions *
 *************************************/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Setup the MPU6050 for motion detection and enable the interrupt.
 * 
 * @param threshold minimum g value to trigger an interrupt.
 * @param duration time in ms where acceleration mus be above threshold.
 */
void setup_mpu6050(uint8_t threshold, uint8_t duration);

/**
 * @brief Clear the MPU6050 status register to reset the external interrupt pin.
 */
void clear_mpu6050_int_status();

#ifdef __cplusplus
}
#endif

#endif
