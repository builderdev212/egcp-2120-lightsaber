#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <Arduino.h>
#include <atmega328p_i2c.h>
#include <adxl345.h>
#include <mcp4725.h>

// Accelerometer Interupt
// uint8_t a_flag = 0;
// ISR(INT0_vect) {
//     a_flag = 1;
// }

int main() {
    // Initalize I2C
    start_i2c();
    Serial.begin(9600);

    // Setup accelerometer
    // if (is_adxl345_present() != 1) return -1;
    // setup_adxl345_activity_interrupt();
    // enable_adxl345();
    write_mcp4725(0x0000);
    Serial.println("sent");

    while (1) {
        for (int i = 0; i < 512; i++) {
            write_mcp4725(i);
            _delay_ms(1000);
            Serial.println("sent");
        }
        // do stuff
        // if (a_flag) {
        //     acceleration fdata = get_current_acceleration();
        //     // do something with data
        //     a_flag = 0;
        // }
    }
}
