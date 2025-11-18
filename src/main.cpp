#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>

#include <atmega328p/i2c.h>
#include <atmega328p/serial.h>
#include <mpu6050.h>
#include <PWMaudio.h>

// Accelerometer Interupt
volatile uint8_t a_flag = 0;
ISR(INT0_vect) {
    a_flag = 1;
}

int main() {
    // Initialize hardware
    uart_init(9600);
    start_i2c();
    setup_mpu6050(2, 1);
    setupPWM();
    setFrequency(100);

    // enable global interrupts
    sei();

    while (1) {
        // play sound if interrupt has been recieved
        if (a_flag) {
            clear_mpu6050_int_status();
            for (int  i = 100; i > 50; i--) {
                setFrequency(i);
                _delay_ms(20);
            }
            for (int  i = 50; i < 101; i++) {
                setFrequency(i);
                _delay_ms(10);
            }
            setFrequency(100);
            a_flag = 0;
        }
    }

    return 0;
}