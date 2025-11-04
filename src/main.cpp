#include <avr/io.h>
#include <avr/interrupt.h>
#include <atmega328p_i2c.h>
#include <adxl345.h>

// Accelerometer Interupt
uint8_t a_flag = 0;
ISR(INT0_vect) {
    a_flag = 1;
}

int main() {
    // Initalize I2C
    start_i2c();

    // Setup accelerometer
    if (is_adxl345_present() != 1) return -1;
    setup_adxl345_activity_interrupt();
    enable_adxl345();

    while (1) {
        // do stuff
        if (a_flag) {
            acceleration fdata = get_current_acceleration();
            // do something with data
            a_flag = 0;
        }
    }
}
