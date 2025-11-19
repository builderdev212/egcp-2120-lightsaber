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

/***************************************
 * PWM Audio User Function Definitions *
 ***************************************/

#ifdef __cplusplus
extern "C" {
#endif

void setupPWM();

void trigger_slash(uint16_t ms);

#ifdef __cplusplus
}
#endif

#endif
