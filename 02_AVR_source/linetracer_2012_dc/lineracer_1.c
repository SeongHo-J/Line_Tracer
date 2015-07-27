#include<avr/io.h>
#include<util/delay.h>
#include<avr/interrupt.h>
#include"MCU_init.h"

#define IDLE 0
#define NORM 1
#define WAIT 2
#define RUN 3

#define IR_NUMBER_m 0
#define IR_NUMBER_M 8
#define DELAY 8
#define RESOLUTION 100

#define Cruze_speed 8000

#define sbi(PORTX,BitX) PORTX|=(1<<BitX) //비트set 명령 정의
#define cbi(PORTX,BitX) PORTX&=~(1<<BitX) //비트clear 명령 정의

// State and Usart variable
volatile int state = 0, i=0, j=0;

// ADC data variable
volatile int ADC_DATA[8] = {0,};
volatile int ADC_MAX[8] = {0,};
volatile int ADC_MIN[8] = {1023,1023,1023,1023,1023,1023,1023,1023};
volatile int NORM_DATA[8] = {0,};
volatile int WEIGHT_DATA[8] = {0,};
volatile float SENSOR_DATA = 0;

void ADC_get(void);
void ADC_max_min(void);
void ADC_normalize(void);
void ADC_weight(void);
void ADC_uart(void);
void Sensor_data(void);

void MOTOR_speed(int lm_speed, int rm_speed);
void MOTOR_control(void);

int main(void) {
	
	init_PORT();
	init_TIMER();
	init_EX_INTERRUPT();
	init_ADC();
	init_USART();
	init_PWM();

	sei();
	
	while(1) {

	}
	
	return 0;
}

// State switch
SIGNAL(INT0_vect) {

	state++;
	if(state>3) state = 3;

}

// State / 20ms control period
SIGNAL(SIG_OVERFLOW3) {
	
	cli();

	switch(state) {
			////////////////////////////////////////////////////////////////////////////////////
			//-------------------------------------IDLE---------------------------------------//
			////////////////////////////////////////////////////////////////////////////////////
			case IDLE :
				
				PORTC = 0x00; // IDLE state check

			break;
			
			
			////////////////////////////////////////////////////////////////////////////////////
			//-------------------------------------NORM---------------------------------------//
			////////////////////////////////////////////////////////////////////////////////////
			case NORM :
				
				PORTC = 0x01; // NORM state check

				ADC_get();

				ADC_uart();

			break;
			
			////////////////////////////////////////////////////////////////////////////////////
			//-------------------------------------WAIT---------------------------------------//
			////////////////////////////////////////////////////////////////////////////////////
			case WAIT :
				
				PORTC = 0x02; // WAIT check



			break;
			
			////////////////////////////////////////////////////////////////////////////////////
			//-------------------------------------RUN----------------------------------------//
			////////////////////////////////////////////////////////////////////////////////////
			case RUN :
				
				PORTC = 0x03; // RUN check
				
			break;
		
	}

	sei();
}

// Get ADC from IR sensor
void ADC_get(void) {

	// 0 - 4 - 1 - 5 - 2 - 6 - 3 - 7 //

	for(i=IR_NUMBER_m ; i<IR_NUMBER_M ; i++) {			
		
		switch(i) {
			
			////////////////////////////////////////////////////////////////////////////////////
			//------------------------------------ADC_0---------------------------------------//
			////////////////////////////////////////////////////////////////////////////////////
			case 0 :
				PORTA = 0x01;

				_delay_us(DELAY);

				ADCSRA |= 0xC0; // ADEN=1, ADSC=1
				while( ((ADCSRA&0x10) == 0x00) ); // wait for ADC complete
				ADC_DATA[0] = ADC;
				
				PORTA = 0x00;
				
				ADMUX = 0xC4; // prepare for ADC_4

			break;
			
			////////////////////////////////////////////////////////////////////////////////////
			//------------------------------------ADC_4---------------------------------------//
			////////////////////////////////////////////////////////////////////////////////////
			case 1 :
				PORTA = 0x10;
				
				_delay_us(DELAY);

				ADCSRA |= 0xC0; // ADEN=1, ADSC=1
				while( ((ADCSRA&0x10) == 0x00) ); // wait for ADC complete
				ADC_DATA[4] = ADC;
				
				PORTA = 0x00;
				
				ADMUX = 0xC1; // prepare for ADC_1

			break;
			
			////////////////////////////////////////////////////////////////////////////////////
			//------------------------------------ADC_1---------------------------------------//
			////////////////////////////////////////////////////////////////////////////////////
			case 2 :
				PORTA = 0x02;
				
				_delay_us(DELAY);

				ADCSRA |= 0xC0; // ADEN=1, ADSC=1
				while( ((ADCSRA&0x10) == 0x00) ); // wait for ADC complete
				ADC_DATA[1] = ADC;
				
				PORTA = 0x00;
				
				ADMUX = 0xC5; // prepare for ADC_5

			break;
			
			////////////////////////////////////////////////////////////////////////////////////
			//------------------------------------ADC_5---------------------------------------//
			////////////////////////////////////////////////////////////////////////////////////
			case 3 :
				PORTA = 0x20;
				
				_delay_us(DELAY);

				ADCSRA |= 0xC0; // ADEN=1, ADSC=1
				while( ((ADCSRA&0x10) == 0x00) ); // wait for ADC complete
				ADC_DATA[5] = ADC;
				
				PORTA = 0x00;
				
				ADMUX = 0xC2; // prepare for ADC_2

			break;
			
			////////////////////////////////////////////////////////////////////////////////////
			//------------------------------------ADC_2---------------------------------------//
			////////////////////////////////////////////////////////////////////////////////////
			case 4 :
				PORTA = 0x04;
				
				_delay_us(DELAY);

				ADCSRA |= 0xC0; // ADEN=1, ADSC=1
				while( ((ADCSRA&0x10) == 0x00) ); // wait for ADC complete
				ADC_DATA[2] = ADC;
				
				PORTA = 0x00;
				
				ADMUX = 0xC6; // prepare for ADC_6

			break;
			
			////////////////////////////////////////////////////////////////////////////////////
			//------------------------------------ADC_5---------------------------------------//
			////////////////////////////////////////////////////////////////////////////////////
			case 5 :
				PORTA = 0x40;
				
				_delay_us(DELAY);
				ADCSRA |= 0xC0; // ADEN=1, ADSC=1
				while( ((ADCSRA&0x10) == 0x00) ); // wait for ADC complete
				ADC_DATA[6] = ADC;
				
				PORTA = 0x00;
				
				ADMUX = 0xC3; // prepare for ADC_3

			break;
			
			////////////////////////////////////////////////////////////////////////////////////
			//------------------------------------ADC_3---------------------------------------//
			////////////////////////////////////////////////////////////////////////////////////
			case 6 :
				PORTA = 0x08;
				
				_delay_us(DELAY);

				ADCSRA |= 0xC0; // ADEN=1, ADSC=1
				while( ((ADCSRA&0x10) == 0x00) ); // wait for ADC complete
				ADC_DATA[3] = ADC;
				
				PORTA = 0x00;
				
				ADMUX = 0xC7; // prepare for ADC_7

			break;
			
			////////////////////////////////////////////////////////////////////////////////////
			//------------------------------------ADC_7---------------------------------------//
			////////////////////////////////////////////////////////////////////////////////////
			case 7 :
				PORTA = 0x80;
				
				_delay_us(DELAY);

				ADCSRA |= 0xC0; // ADEN=1, ADSC=1
				while( ((ADCSRA&0x10) == 0x00) ); // wait for ADC complete
				ADC_DATA[7] = ADC;
				
				PORTA = 0x00;
				
				ADMUX = 0xC0; // prepare for ADC_0

			break;
		}
		
	}
	
}

// Get ADC max and min value from ADC_DATA[i]
void ADC_max_min(void) {
	
	for(i=IR_NUMBER_m ; i<IR_NUMBER_M ; i++) {					
		if( ADC_MAX[i] < ADC_DATA[i] ) // ADC_MAX
			ADC_MAX[i] = ADC_DATA[i];
		if( ADC_MIN[i] > ADC_DATA[i]) // ADC_MIN
			ADC_MIN[i] = ADC_DATA[i];	
	}

}

// Get normalized ADC value (white base / black line / resolution=100)
void ADC_normalize(void) {
	
	double temp_1=0, temp_2=0;

	for(i=IR_NUMBER_m ; i<IR_NUMBER_M ; i++) {

		temp_1 = ADC_MAX[i] - ADC_DATA[i];
		temp_2 = ADC_MAX[i] - ADC_MIN[i];

		NORM_DATA[i] = (int)((RESOLUTION*temp_1)/temp_2);

		if(i==5) { // correct error
			NORM_DATA[5] -= 5;

			if(NORM_DATA[5] < 0)
				NORM_DATA[5] += 5;
		}

	}

}

// Get Weighted value for normalize data(NORM_DATA[i])
void ADC_weight(void) {

	int weight[8] = {30, 20, 1, 0, 0, -1, -20, -30};
	
	for(i=IR_NUMBER_m ; i<IR_NUMBER_M ; i++) {

		WEIGHT_DATA[i] = NORM_DATA[i] * weight[i];
	
	}

}

// Get sensor_data from weighted data(WEIGHT_DATA[i])
void Sensor_data(void) {

	int count_sensor=0, temp=0;

	// count the number of sensor determined that it is sensed
	for(i=IR_NUMBER_m ; i<IR_NUMBER_M ; i++) {
		
		if((NORM_DATA[i] >= 70) && (i!=3) && (i!=4)) count_sensor++;
		// 50 is a min value that can be thought that it is sensed

	}


	for(i=IR_NUMBER_m ; i<IR_NUMBER_M ; i++) {
		
		temp += WEIGHT_DATA[i]; // the sum of weighted data for finding error

	}

	if(count_sensor == 0) 
		SENSOR_DATA = 0;
	else 
		SENSOR_DATA = (float)(temp / count_sensor);

}

void MOTOR_speed(int lm_speed, int rm_speed) {	
	
	// because the maximum value of OCR1A and OCR1B is 16000 in the setting
	if(lm_speed > 15500) lm_speed = 16000;
	
	else if(lm_speed < -15500) lm_speed = -16000;

	if(rm_speed > 15500) rm_speed = 16000;
	
	else if(rm_speed < -15500) rm_speed = -16000;


	if(lm_speed > 0) {
		sbi(PORTC, PORTC2); // nomal direction
		OCR1A = lm_speed;
	}
	else if(lm_speed < 0) {
		cbi(PORTC, PORTC2); // reverse direction
		OCR1A = -1 * lm_speed;
	}
	
	if(rm_speed > 0) {
		cbi(PORTC, PORTC3); // nomal direction
		OCR1B = rm_speed;
	}
	else if(rm_speed < 0 ) {
		sbi(PORTC, PORTC3); // reverse direction
		OCR1B = -1 * rm_speed;
	}

}

void MOTOR_control(void) {
	
	if(SENSOR_DATA > 0 ) {
		MOTOR_speed(Cruze_speed + SENSOR_DATA, Cruze_speed - SENSOR_DATA);
	}
	else if(SENSOR_DATA < 0) {
		MOTOR_speed(Cruze_speed + SENSOR_DATA, Cruze_speed + (-1*SENSOR_DATA));
	}

}

void ADC_uart(void) {
	
	//---------------ADC_DATA[i] output---------------//
	Uart_Putch0('D');
	Uart_Putch0('a');
	Uart_Putch0('t');
	Uart_Putch0('\t');

	for(i=IR_NUMBER_m ; i<IR_NUMBER_M ; i++) {			
		
		Uart_U16Bit_PutNum0(ADC_DATA[i]);
		Uart_Putch0('\t');

	}

	Uart_Putch0('\n');
	Uart_Putch0('\r');
/*	
	//---------------ADC_MAX[i] output---------------//
	Uart_Putch0('M');
	Uart_Putch0('a');
	Uart_Putch0('x');	
	Uart_Putch0('\t');

	for(i=IR_NUMBER_m ; i<IR_NUMBER_M ; i++) {
		
		Uart_U16Bit_PutNum0(ADC_MAX[i]);
		Uart_Putch0('\t');

	}

	Uart_Putch0('\n');
	Uart_Putch0('\r');

	//---------------ADC_MIN[i] output---------------//
	Uart_Putch0('M');
	Uart_Putch0('i');
	Uart_Putch0('n');
	Uart_Putch0('\t');

	for(i=IR_NUMBER_m ; i<IR_NUMBER_M ; i++) {
		
		Uart_U16Bit_PutNum0(ADC_MIN[i]);
		Uart_Putch0('\t');
	
	}

	Uart_Putch0('\n');
	Uart_Putch0('\r');

	//---------------NORM_DATA[i] output---------------//
	Uart_Putch0('N');
	Uart_Putch0('o');
	Uart_Putch0('m');
	Uart_Putch0('\t');
	
	for(i=IR_NUMBER_m ; i<IR_NUMBER_M ; i++) {
		
		Uart_U16Bit_PutNum0(NORM_DATA[i]);
		Uart_Putch0('\t');

	}

	Uart_Putch0('\n');
	Uart_Putch0('\r');

	//---------------WEIGHT_DATA[i] output---------------//
	Uart_Putch0('W');
	Uart_Putch0('e');
	Uart_Putch0('i');
	Uart_Putch0('\t');

	for(i=IR_NUMBER_m ; i<IR_NUMBER_M ; i++) {
		
		if(WEIGHT_DATA[i] >= 0) {

			Uart_U16Bit_PutNum0(WEIGHT_DATA[i]);
		}
		else {
			WEIGHT_DATA[i] = -1 * WEIGHT_DATA[i]; 
			Uart_Putch0('-');
			Uart_U16Bit_PutNum0(WEIGHT_DATA[i]);

			WEIGHT_DATA[i] = -1 * WEIGHT_DATA[i];
			
		}

		Uart_Putch0('\t');

	}

	Uart_Putch0('\n');
	Uart_Putch0('\r');


	//---------------SENSOR_DATA output---------------//
	Uart_Putch0('S');
	Uart_Putch0('e');
	Uart_Putch0('n');
	Uart_Putch0('\t');
	
	if(SENSOR_DATA >=0) {
		Uart_U16Bit_PutNum0(SENSOR_DATA);
	}
	else {
		SENSOR_DATA = -1*SENSOR_DATA;
		Uart_Putch0('-');
		Uart_U16Bit_PutNum0(SENSOR_DATA);
		SENSOR_DATA = -1*SENSOR_DATA;
	}

	Uart_Putch0('\n');
	Uart_Putch0('\r');

	//---------------MOTOR_speed output---------------//
	Uart_Putch0('M');
	Uart_Putch0('o');
	Uart_Putch0('t');
	Uart_Putch0('\t');
	

	Uart_U16Bit_PutNum0(Cruze_speed+SENSOR_DATA);

	Uart_Putch0('\t');

	Uart_U16Bit_PutNum0(Cruze_speed-SENSOR_DATA);*/

	Uart_Putch0('\n');
	Uart_Putch0('\r');

}
