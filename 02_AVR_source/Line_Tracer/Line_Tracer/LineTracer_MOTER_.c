#define F_CPU 16000000

#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include "LineTracer_MOTER.h"
#include "linetracer_ADC.h"

void init_MCU(void)
{
	DDRC = 0xff;
	PORTC = 0x00;
	DDRD = 0xff;
	PORTD = 0x30;
	
}

void init_PWM(void)
{
	DDRB = 0xFF;
	TCCR0 = 0x6F;
	TCCR2 = 0x6D;
	TCNT0 = 0xff;
	TCNT2 = 0xff;
	OCR0 = 0xff;
	OCR2 = 0xff;
	
}

void MOTER_test(const int *NORM_DATA){
		
	if(NORM_DATA[0]>=(int)(RESOLUTION*0.5)&&NORM_DATA[7]<=(int)(RESOLUTION*0.5)){
		PORTC = 0x04;		
	}
	else if(NORM_DATA[0]<=(int)(RESOLUTION*0.5)&&NORM_DATA[7]>=(int)(RESOLUTION*0.5)){
		PORTC = 0x40;		
	}
	else if(NORM_DATA[0]>=(int)(RESOLUTION*0.5)&&NORM_DATA[7]>=(int)(RESOLUTION*0.5)){
		PORTC = 0x44;		
	}
	else{
		PORTC = 0x00;	
	}
	
}