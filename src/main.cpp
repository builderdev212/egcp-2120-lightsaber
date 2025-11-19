#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>
#include <atmega328p/i2c.h>
#include <atmega328p/serial.h>
#include <mpu6050.h>
#include <PWMaudio.h>
#include <ws2812b.h>

// Accelerometer Interupt
ISR(INT0_vect) {            
    clear_mpu6050_int_status();
    trigger_slash(1000);
}

int main()
{
    // Initialize hardware
    DDRD |= (1 << PD3);
    uart_init(9600);
    start_i2c();
    setup_mpu6050(2, 1);
    setupPWM();

    unsigned char R[60] = {
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 230, 204, 178, 153, 128, 102, 77, 51, 26,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 25, 51, 76, 102, 128, 153, 179, 204, 230,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255};

    unsigned char G[60] = {
        0, 25, 51, 77, 102, 128, 153, 178, 204, 229,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 230, 204, 178, 153, 128, 102, 76, 51, 25,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    unsigned char B[60] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 25, 51, 76, 102, 128, 153, 179, 204, 229,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 230, 204, 179, 153, 128, 102, 77, 51, 26};

    SendArrayRGB(R, G, B, 60);
    _delay_ms(500);

    // enable global interrupts
    sei();

    while (1) {
        
    }

    return 0;
}
