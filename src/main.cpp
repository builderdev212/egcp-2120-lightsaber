#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>
#include <atmega328p/i2c.h>
#include <atmega328p/serial.h>
#include <mpu6050.h>
#include <mcp4725.h>

// Sound files
#include "swing3.h"
#define SWING3_LEN 7746

// Accelerometer Interupt
uint8_t a_flag = 0;
ISR(INT0_vect) {
    a_flag = 1;
}

int main() {
    // Initialize hardware
    uart_init(9600);
    start_i2c();
    setup_mpu6050(5, 1);
    write_mcp4725(0x000);

    while (1) {
        if (a_flag) {
            uart_print("MOTION!!!");
            for (int i = 0; i < SWING3_LEN; i++) {
                write_mcp4725((uint16_t)swing3[i] << 4);
            }
            clear_mpu6050_int_status();
            a_flag = 0;
        }
        _delay_ms(1);
    }
}
