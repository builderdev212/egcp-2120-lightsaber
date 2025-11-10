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
uint16_t i = 0;
ISR(INT0_vect) {
    a_flag = 1;
    start_timer1();
    i = 0;
}

// WAV Timing Interrupt
uint8_t sready = 0;
ISR(TIMER1_COMPA_vect) {
    sready = 1;
}

int main() {
    // Initialize hardware
    uart_init(9600);
    start_i2c();
    setup_mpu6050(5, 1);
    setup_timer1_4khz();
    write_mcp4725(0x000);

    // enable global interrupts
    sei();

    while (1) {
        // if the timer is polled, play current audio
        if (sready) {
            sready = 0;
            write_mcp4725((uint16_t)swing3[i] << 4);
            i++;
        }
        // play sound if interrupt has been recieved
        if (a_flag) {
            if (i == SWING3_LEN) {
                clear_mpu6050_int_status();
                stop_timer1();
                a_flag = 0;
                i = 0;
            }
        }
    }
}
