#include<avr/io.h>
#include<util/delay.h>
#include<avr/interrupt.h>

#define IDLE 0
#define NORM 1
#define WAIT 2
#define RUN 3

volatile int state=0;

void init_timer(void);
void init_ex_interrupt(void);

SIGNAL(INT0_vect) {
	
	state++;
	
	if(state>3){
		state = 0;
	}

}

SIGNAL(TIMER_OVF3) { // 20ms
	cli();

	sei();
}

int main(void) {

	DDRC = 0x0F;
	DDRD = 0x00;

	PORTC = 0x03;

	
	init_timer();
	init_ex_interrupt();

	sei();

	while(1) {

		switch(state) {
		
		case IDLE :
			PORTC = 0x03;

		break;

		case NORM :
			PORTC = 0x02;

		break;

		case WAIT :
			PORTC = 0x01;

		break;

		case RUN :
			PORTC = 0x00;

		break;
		
	}


	}

	return 0;
}

void init_ex_interrupt(void) {
	
	EICRA = 0x02;
	EIMSK = 0x01;
}

void init_timer(void) {
	
	TCCR3A = 0x02;
	TCCR3B = 0x1A;
	ETIMSK = 0x04;

	ICR3 = 40000;

}
