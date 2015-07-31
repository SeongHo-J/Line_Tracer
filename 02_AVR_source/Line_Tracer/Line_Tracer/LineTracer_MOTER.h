#ifndef _LINETRACER_MOTER_H_
#define _LINETRACER_MOTER_H_

#define F_CPU 16000000
#include<util/delay.h>
#include<avr/interrupt.h>
#include<avr/io.h>

/* Bit set or clear */
#define sbi(PORTX,BitX) PORTX|=(1<<BitX)	// Bit set function
#define cbi(PORTX,BitX)	PORTX&=~(1<<BitX)	// Bit clear fuction

void MOTER_test(const int *NORM_DATA);
void MOTOR_Lspeed(int LM_speed, const double *SENSOR_DATA);
void MOTOR_Rspeed(int RM_speed, const double *SENSOR_DATA);
void MOTOR_control(const double *SENSOR_DATA, const int Cruze_speed); 

#else
#endif