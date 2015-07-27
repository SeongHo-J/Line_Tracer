#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>

#include"MCU_init.h"
#include"linetracer_ADC.h"

#define IDLE 0
#define NORM 1
#define WAIT 2
#define RUN 3

#define CRUZE_SPEED 0x49

#define Kp 1.3
#define Kd 0.8

volatile int state=IDLE;

int ADC_DATA[4] = {0,};
int ADC_MAX[4] = {0,};
int ADC_MIN[4] = {1023,1023,1023,1023};
int NORM_DATA[4] = {0,};
int WEIGHT_DATA[4] = {0,};

volatile int SENSOR_DATA=0;

int main(void) {

	// Local Variable //
	float err=0, pre_err=0;
	float diff_err=0;
	float MV=0;

	// Initialize Function //
	init_PORT();
	init_EX_INTERRUPT();
	init_PWM();
	init_ADC();

	sei();

	while(1) {

		
		switch(state) {
				////////////////////////////////////////////////////////////////////////////////////
				//-------------------------------------IDLE---------------------------------------//
				////////////////////////////////////////////////////////////////////////////////////
				case IDLE :

					PORTD = 0x03; // IDLE state check

					break;
			
			
				////////////////////////////////////////////////////////////////////////////////////
				//-------------------------------------NORM---------------------------------------//
				////////////////////////////////////////////////////////////////////////////////////
				case NORM :

					PORTD = 0x02; // NORM state check

					ADC_GET(ADC_DATA);

					ADC_MAX_MIN(ADC_MAX, ADC_MIN, ADC_DATA);

					break;
			
				////////////////////////////////////////////////////////////////////////////////////
				//-------------------------------------WAIT---------------------------------------//
				////////////////////////////////////////////////////////////////////////////////////
				case WAIT :

					PORTD = 0x01; // WAIT check

					TCCR1A	|= (1<<COM1A1)|(0<<COM1A0)|(1<<COM1B1)|(0<<COM1B0);

					break;
			
				////////////////////////////////////////////////////////////////////////////////////
				//-------------------------------------RUN----------------------------------------//
				////////////////////////////////////////////////////////////////////////////////////
				case RUN :

					PORTD = 0x00; // RUN check

					ADC_GET(ADC_DATA);

					ADC_NORMALIZE(NORM_DATA, ADC_DATA, ADC_MAX, ADC_MIN);

					DATA_WEIGHT(&SENSOR_DATA, WEIGHT_DATA, NORM_DATA);

					//---------------- PD controller ----------------//
					// Err = Reference valuse - output
					// MV  = Kp*Err + Kd*(Err - pre_Err)

					err = (float)(-SENSOR_DATA); // Reference value is 0

					diff_err = err - pre_err;
					pre_err = err;

					MV = Kp*err + Kd*diff_err;

					//------------------------------------------------//

					MV = MV/20;

					if(SENSOR_DATA<0) {				

						OCR1A = CRUZE_SPEED + MV;
						OCR1B = CRUZE_SPEED - MV;

					}
					else if(SENSOR_DATA>0) {

						OCR1A = CRUZE_SPEED + MV;
						OCR1B = CRUZE_SPEED - MV;

					}
					else{

						OCR1A = CRUZE_SPEED;
						OCR1B = CRUZE_SPEED;

					}

					SENSOR_DATA=0;
													
					break;
		}

		_delay_ms(3);
	}

	return 0;

}

SIGNAL(INT0_vect) {

	state++;

	if(state>3) state = 3;

}
