#include<abr/io.h>

volatile int state = 0;
#define IDLE 0
#define NORM 1
#define WAIT 2
#define RUN 3

void Timer_init(void);
void Exteranl_init(void);

SIGNAL(Interrupt0) {
	
	state++;

	if(state > 3) {
		state = 3;
	}

}                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                

SIGNAL(TIMER_OVF3) { // 20ms

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

}

int main(void) {
	
	DDRC = 0x0F; // PC0=LED0, PC1=LED1, PC2=DIR_right, PC3=DIR_left
	DDRD = 0x00; // PD0=INT0
	
	PORTC = 0x00;
	PORTD = 0xFF;

	Timer_init();

	while(1) {

		

	}

	return 0;
}

void TImer_init(void) { // control period
	
	TCCR3A = 0b00000010;
	TCCR3B = 0b00011010;
	ETIMSK = 0b00000100;

}
