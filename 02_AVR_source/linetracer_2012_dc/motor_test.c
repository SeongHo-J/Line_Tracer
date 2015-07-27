#include<avr/io.h>
#include<util/delay.h>
#include<avr/interrupt.h>
#include"MCU_init.h"

#define IDLE 0
#define NORM 1
#define WAIT 2
#define RUN 3

volatile int state=0;

SIGNAL(INT0_vect) {

	state++;

	if(state>3) state = 3;

}

SIGNAL(SIG_OVERFLOW3) { // 20ms
	cli();

	switch(state) {
		
			case IDLE :
				PORTC = 0x00;

			break;

			case NORM :
				PORTC = 0x01;

			break;

			case WAIT :
				PORTC = 0x02;

			break;

			case RUN :
				PORTC = 0x03;

			break;
		
	}

	sei();
}

int main(void) {
	
	DDRB = 0xF0;	
	
	TCCR0 = 0x6F;
	TCNT0 = 0xFF;	

	init_TIMER();
	init_EX_INTERRUPT();
	init_PWM();

	sei();
	
	while(1) {

	    OCR0 = 0x40;

		OCR1AL = 0x40;
		OCR1BL = 0x08;

	}

	return 0;

}
