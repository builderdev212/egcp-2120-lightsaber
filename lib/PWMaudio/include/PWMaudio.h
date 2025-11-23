#ifndef PWMAUDIO_H
#define PWMAUDIO_H

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <stdlib.h>

/***********************
 * PWM Audio Constants *
 ***********************/

#define PWM_PORT PORTB
#define PWM_PORT_IO_CONF DDRB
#define PWM_PIN 0x02

#ifndef F_CPU
#define F_CPU 16000000UL
#endif
#define SAMPLE_RATE 8000UL

#define SLASH_BASE_FREQ   300
#define SLASH_END_FREQ   1500
#define SLASH_TONE_GAIN    60
#define SLASH_NOISE_GAIN   80

/***************************************
 * PWM Audio User Function Definitions *
 ***************************************/

#ifdef __cplusplus
extern "C" {
#endif

void setupPWM();

void setInc(uint8_t out);

#ifdef __cplusplus
}
#endif

#endif
