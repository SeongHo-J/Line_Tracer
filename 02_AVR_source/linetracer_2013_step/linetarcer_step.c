#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>

#include"MCU_init.h"
#include"linetracer_ADC.h"
#include"linetracer_MOTOR.h"
#include"linetracer_UART.h"

#define IDLE 0
#define NORM 1
#define WAIT 2
#define RUN 3

#define CRUZE_SPEED 200

#define Kp 1.0
#define Kd 0.5

int state=IDLE;

int ADC_DATA[8] = {0,};
int ADC_MAX[8] = {0,};
int ADC_MIN[8] = {1023,1023,1023,1023,1023,1023,1023,1023};
int NORM_DATA[8] = {0,};
int WEIGHT_DATA[8] = {0,};

int DIRECTION=0;
int END_FLAG = 0;

float err=0, pre_err=0;
float diff_err=0;
float MV=0;

char step;

int main(void) {

	// Local Variable //
	char a=0;
	

	// Initialize Function //
	init_PORT();
	init_EX_INTERRUPT();
	init_TIMER0();
	init_TIMER3();
	init_ADC();
	//init_USART();

	sei();

	while(1) {

	}



	return 0;

}

// State switch
SIGNAL(INT4_vect) {

	state++;

	if(state>3) state = 3;

}

// Motor
ISR(TIMER0_COMP_vect) {

	err = -DIRECTION; // Reference value is 0

	diff_err = err - pre_err;
	pre_err = err;

	MV = Kp*err + Kd*diff_err;

	MV = MV/10;
	
	if(DIRECTION==0) MOTOR(&step, FORWARD, CRUZE_SPEED);
	else if(DIRECTION>0) MOTOR(&step, RIGHT, CRUZE_SPEED-MV);
	else if(DIRECTION<0) MOTOR(&step, LEFT, CRUZE_SPEED+MV);
	
	return;

}

// State / 10ms control period
SIGNAL(SIG_OVERFLOW3) {

	cli();

	//int i=0;

	switch(state) {
			////////////////////////////////////////////////////////////////////////////////////
			//-------------------------------------IDLE---------------------------------------//
			////////////////////////////////////////////////////////////////////////////////////
			case IDLE :

				PORTA = 0x01; // IDLE state check

				TIMSK &= ~(1<<OCIE0);
				PORTC = 0x00;

				break;
			
			
			////////////////////////////////////////////////////////////////////////////////////
			//-------------------------------------NORM---------------------------------------//
			////////////////////////////////////////////////////////////////////////////////////
			case NORM :

				PORTA = 0x02; // NORM state check
				
				ADC_GET(ADC_DATA);

				ADC_MAX_MIN(ADC_MAX, ADC_MIN, ADC_DATA);

				break;
			
			////////////////////////////////////////////////////////////////////////////////////
			//-------------------------------------WAIT---------------------------------------//
			////////////////////////////////////////////////////////////////////////////////////
			case WAIT :

				PORTA = 0x04; // WAIT check

				break;
			
			////////////////////////////////////////////////////////////////////////////////////
			//-------------------------------------RUN----------------------------------------//
			////////////////////////////////////////////////////////////////////////////////////
			case RUN :

				PORTA = 0x08; // RUN check

				DIRECTION=0;

				ADC_GET(ADC_DATA);

				ADC_NORMALIZE(NORM_DATA, ADC_DATA, ADC_MAX, ADC_MIN);

				DATA_WEIGHT(&DIRECTION, WEIGHT_DATA, NORM_DATA);

				TIMSK |= (1<<OCIE0);

				//-----------------------------//
				//----------- Break -----------//
				//-----------------------------//
				if( WEIGHT_DATA[2]==0 && WEIGHT_DATA[5]==0 && WEIGHT_DATA[0]!=0 && WEIGHT_DATA[7]!=0 ) {
					
					if(END_FLAG == 2) state = IDLE;					
					END_FLAG = 1;
				
				}

				if(END_FLAG==1 && WEIGHT_DATA[0]==0 && WEIGHT_DATA[7]==0) END_FLAG=2;
				//-----------------------------//

				break;
	}

	sei();

}
