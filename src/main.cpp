#include <avr/io.h>
#include <atmega328p_i2c.h>
#include <adxl345.h>

int main() {
    // Initalize I2C
    start_i2c();

    // Setup accelerometer
    if (is_adxl345_present() != 1) return -1;
    enable_adxl345();

    while (1) {

    }
}
