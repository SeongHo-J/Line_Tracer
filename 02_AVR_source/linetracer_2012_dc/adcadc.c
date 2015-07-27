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

// State and Usart variable
volatile int state = 0, i=0, j=0;

// ADC data variable
volatile int ADC_DATA[8] = {0,};

void ADC_get(void);
void ADC_uart(void);

int main(void) {
	
	init_PORT();
	init_TIMER();
	init_EX_INTERRUPT();
	init_ADC();
	init_USART();
	init_PWM();

	sei();
	
	Uart_Putch0('0');

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
