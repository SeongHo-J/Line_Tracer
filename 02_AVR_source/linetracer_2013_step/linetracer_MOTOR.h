#ifndef _LINETRACER_MOTOR_H_
#define _LINETRACER_MOTOR_H_

#include<avr/io.h>
#include<util/delay.h>

#define PI 100 // 2 phase excitation = 100 / 1-2 phase excitation = 200
#define RS 3

#define PH_EX 4 // 2 phase excitation = 4 / 1-2 phase excitation = 8

#define FORWARD 0
#define BACKWARD 1
#define RIGHT 2
#define LEFT 3

void MOTOR_R(const int dir, const int rad);
void MOTOR_L(const int dir, const int rad);
//void MOTOR(const int dir, const int pulse, const int speed);
void MOTOR(char *step, const char dir, const int speed);

#else
#endif
