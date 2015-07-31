#define F_CPU 16000000

#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include "linetracer_ADC.h"
#include "LineTracer_MOTER.h"
#include "LineTracer_MCU_init.h"


#define IDLE 0 // 대기모드
#define NORM 1 // 측정모드
#define WAIT 2 // 대기모드
#define RUN 3 // 작동모드


// ADC data variable
int ADC_DATA[8] = {0,};
int ADC_MAX[8] = {0,};
int ADC_MIN[8] = {1023,1023,1023,1023,1023,1023,1023,1023};
int NORM_DATA[8] = {0,};
int WEIGHT_DATA[8] = {0,};
double SENSOR_DATA[1] = {0};

int state = 0;

int LM_speed = 0;
int RM_speed = 0;
int Cruze_speed = 0xc0;

int main(void)
{
	init_PWM();
	init_MCU();
	init_ADC();
	init_EX_INTERRUPT();
	
	sei();
	
	while(1)
	{
		
		switch(state) {
					////////////////////////////////////////////////////////////////////////////////////
					//-------------------------------------IDLE---------------------------------------//
					////////////////////////////////////////////////////////////////////////////////////
					case IDLE :
						PORTD = 0x00; // IDLE state check
					break;
			
			
					////////////////////////////////////////////////////////////////////////////////////
					//-------------------------------------NORM---------------------------------------//
					////////////////////////////////////////////////////////////////////////////////////
					case NORM :
						PORTD = 0x10; // NORM state check
						ADC_GET(ADC_DATA);
						ADC_MAX_MIN(ADC_MAX, ADC_MIN, ADC_DATA);		

					break;
			
					////////////////////////////////////////////////////////////////////////////////////
					//-------------------------------------WAIT---------------------------------------//
					////////////////////////////////////////////////////////////////////////////////////
					case WAIT :
						PORTD = 0x20; // WAIT check

					break;	
			
					////////////////////////////////////////////////////////////////////////////////////
					//-------------------------------------RUN----------------------------------------//
					////////////////////////////////////////////////////////////////////////////////////
					case RUN :
						PORTD = 0x30; // RUN check						

						ADC_GET(ADC_DATA);
						ADC_NORMALIZE(NORM_DATA, ADC_DATA, ADC_MAX, ADC_MIN);
						//MOTER_test(NORM_DATA);
						ADC_WEIGHT(WEIGHT_DATA, NORM_DATA);
						ADC_SENCERDATA(SENSOR_DATA, WEIGHT_DATA, NORM_DATA);
						MOTOR_control(SENSOR_DATA, Cruze_speed);						
				
					break;
				}		
		
	}
	
}

// State switch
SIGNAL(INT0_vect) {

	cli();	
	state++;
	if(state>3) state = 3;
	sei();
}



