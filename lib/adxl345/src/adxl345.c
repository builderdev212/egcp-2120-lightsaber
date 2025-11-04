#include "adxl345.h"

/****************
 * Register Map *
 ****************/

#define ADXL345_ID_ADDR 0x00
#define ADXL345_POWER_CTL 0x2D
#define ADXL345_DATA_FORMAT 0x31
#define ADXL345_DATA_START 0x32
#define ADXL345_BR 0x2C

/***************************
 * ADXL345 MACRO Functions *
 ***************************/

#define CONCAT_CAST_2xU8_TO_S16(val1, val2) (int16_t)(((uint16_t)val2 << 8) | (uint16_t)val1)

/**************************
 * ADXL345 User Functions *
 **************************/

uint8_t is_adxl345_present() {
    uint8_t device_id[1] = {0x00};
    addr_read_i2c(ADXL345_ADDR, ADXL345_ID_ADDR, device_id, 1);
    if (device_id[0] != ADXL345_ID) return 0;
    return 1;
}

void enable_adxl345() {
    // setup power control(int16_t) register
    uint8_t pcwdata[2] = {ADXL345_POWER_CTL, 0x08};
    write_i2c(ADXL345_ADDR, pcwdata, 2);
    // setup the device bit rate (12.5Hz)
    uint8_t brwdata[2] = {ADXL345_BR, 0x07};
    write_i2c(ADXL345_ADDR, brwdata, 2);
    // setup the data format
    uint8_t dfwdata[2] = {ADXL345_DATA_FORMAT, 0x00};
    write_i2c(ADXL345_ADDR, dfwdata, 2);
    // setup the interrupt pin (INT0)
    EICRA = 0x03;
    EIMSK = 0x01;
    INT_PORT |= ADXL345_INT_PIN;
    INT_PORT_IO_CONF &= ~(ADXL345_INT_PIN);
}

struct acceleration get_current_acceleration() {
    uint8_t adata[6];
    addr_read_i2c(ADXL345_ADDR, ADXL345_DATA_START, adata, 6);
    struct acceleration curr;
    curr.x = CONCAT_CAST_2xU8_TO_S16(adata[1], adata[0]);
    curr.y = CONCAT_CAST_2xU8_TO_S16(adata[3], adata[2]);
    curr.z = CONCAT_CAST_2xU8_TO_S16(adata[5], adata[4]);
    return curr;
}
