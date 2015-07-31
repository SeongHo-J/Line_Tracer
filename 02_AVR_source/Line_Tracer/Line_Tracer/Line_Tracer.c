#define F_CPU 16000000

#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include "linetracer_ADC.h"
#include "LineTracer_MOTER.h"

int ADC_DATA[8] = {0,};
int ADC_MAX[8] = {0,};
int ADC_MIN[8] = {1023,1023,1023,1023,1023,1023,1023,1023};
int NORM_DATA[8] = {0,};

int main(void)
{
	init_PWM();
	init_MCU();
	init_ADC();
	
	while(1)
	{
		ADC_GET(ADC_DATA);
		ADC_MAX_MIN(ADC_MAX, ADC_MIN, ADC_DATA);
		ADC_NORMALIZE(NORM_DATA, ADC_DATA, ADC_MAX, ADC_MIN);
		MOTER_test(NORM_DATA);			
		
	}
	
}



