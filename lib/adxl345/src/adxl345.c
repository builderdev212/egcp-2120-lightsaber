#include "adxl345.h"

/****************
 * Register Map *
 ****************/

#define ADXL345_ID_ADDR 0x00
#define ADXL345_POWER_CTL 0x2D
#define ADXL345_DATA_FORMAT 0x31

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
    // setup power control register
    uint8_t pcrdata[2] = {ADXL345_POWER_CTL, 0x08};
    write_i2c(ADXL345_ADDR, pcrdata, 2);
    // setup the data format
    uint8_t dfrdata[2] = {ADXL345_DATA_FORMAT, 0x00};
    write_i2c(ADXL345_ADDR, dfrdata, 2);
}
