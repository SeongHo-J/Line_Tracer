#define F_CPU 16000000

#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/io.h>

void init_PWM(void);
void init_MCU(void);
void MOTER_test(const int *NORM_DATA);