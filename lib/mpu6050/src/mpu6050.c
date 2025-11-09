#include "mpu6050.h"

void setup_mpu6050(uint8_t threshold, uint8_t duration) {
    // reset device/wake it up
    write_i2c(MPU6050_ADDR, (uint8_t[]){MPU6050_PWR_MGMT_1, 0x00}, 2);
    // configure measurement range (+/-8g) and sample rate
    write_i2c(MPU6050_ADDR, (uint8_t[]){MPU6050_ACCEL_CONFIG, 0x10}, 2);
    write_i2c(MPU6050_ADDR, (uint8_t[]){MPU6050_CONFIG, 0x00}, 2);
    write_i2c(MPU6050_ADDR, (uint8_t[]){MPU6050_SMPLRT_DIV, 0x00}, 2);
    // setup motion interrupt
    write_i2c(MPU6050_ADDR, (uint8_t[]){MPU6050_MOT_THR, threshold}, 2);
    write_i2c(MPU6050_ADDR, (uint8_t[]){MPU6050_MOT_DUR, duration}, 2);
    write_i2c(MPU6050_ADDR, (uint8_t[]){MPU6050_MOT_DETECT_CTRL, 0x15}, 2);
    // enable interrupts
    write_i2c(MPU6050_ADDR, (uint8_t[]){MPU6050_INT_PIN_CONFIG, 0x80}, 2);
    write_i2c(MPU6050_ADDR, (uint8_t[]){MPU6050_INT_EN, 0x40}, 2);
    // setup the interrupt pin (INT0)
    EICRA = (EICRA & ~(0x03)) | 0x02;
    EIMSK |= 0x01;
    INT_PORT |= MPU6050_INT_PIN;
    INT_PORT_IO_CONF &= ~(MPU6050_INT_PIN);
    // enable global interrupts
    sei();
}

void clear_mpu6050_int_status() {
    uint8_t status = 0;
    addr_read_i2c(MPU6050_ADDR, MPU6050_INT_STATUS, &status, 1);
    _delay_ms(50);
}
