#include "adxl345.h"

/****************
 * Register Map *
 ****************/

#define ADXL345_ID_ADDR 0x00
#define ADXL345_POWER_CTL 0x2D

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
    uint8_t wrdata[2] = {ADXL345_POWER_CTL, 0x08};
    write_i2c(ADXL345_ADDR, wrdata, 2);
}
