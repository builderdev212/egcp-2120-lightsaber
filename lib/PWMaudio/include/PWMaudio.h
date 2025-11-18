#ifndef PWMAUDIO_H
#define PWMAUDIO_H

#include <avr/io.h>
#include <stdlib.h>

/***********************
 * PWM Audio Constants *
 ***********************/

#define PWM_PORT PORTB
#define PWM_PORT_IO_CONF DDRB
#define PWM_PIN 0x02

/***************************************
 * PWM Audio User Function Definitions *
 ***************************************/

#ifdef __cplusplus
extern "C" {
#endif

void setupPWM();

void setFrequency(uint16_t freq_hz);

#ifdef __cplusplus
}
#endif

#endif
