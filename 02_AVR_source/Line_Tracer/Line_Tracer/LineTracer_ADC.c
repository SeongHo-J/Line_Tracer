#define F_CPU 16000000

#include <avr/io.h>
#include <util/delay.h>
#include "linetracer_ADC.h"


/*
int main(void) {
	
	DDRA = 0xFF;
	DDRE = 0xFF;
	DDRF = 0x00;
	
	PORTA = 0x00;
		
	ADMUX = 0x00;
	ADCSRA = 0x87;
	
	while(1) {
		
		ADC_GET(ADC_DATA);
		ADC_MAX_MIN(ADC_MAX, ADC_MIN, ADC_DATA);
		ADC_NORMALIZE(NORM_DATA, ADC_DATA, ADC_MAX, ADC_MIN);
		DATA_OUT(NORM_DATA);	
						
	}

	return 0;
}
*/
void init_ADC(void){
	DDRA = 0xFF;
	DDRE = 0xFF;
	DDRF = 0x00;
	
	ADMUX = 0xC0;
	ADCSRA = 0x87;
}

void ADC_GET(int *ADC_DATA) {

	//------------IR order-----------//
	// 0 - 1 - 2 - 3 - 4 - 5 - 6 - 7 //
	
	int i=0;
		
	for(i=IR_NUMBER_m ; i<=IR_NUMBER_M ; i++) {

		switch(i) {

			////////////////////////////////////////////////////////////////////////////////////
			//------------------------------------ADC_0---------------------------------------//
			////////////////////////////////////////////////////////////////////////////////////
			case 0 :		
			
			PORTA = 0x01;

			ADCSRA |= 0xC0; // ADEN=1, ADSC=1
			while( ((ADCSRA&0x10) == 0x00) ); // wait for ADC complete

			ADC_DATA[0] = ADC;
			
			PORTA = 0x00;

			ADMUX = 0xC1; // prepare for ADC_1

			break;

			////////////////////////////////////////////////////////////////////////////////////
			//------------------------------------ADC_1---------------------------------------//
			////////////////////////////////////////////////////////////////////////////////////
			case 1 :
			PORTA = 0x02;

			ADCSRA |= 0xC0; // ADEN=1, ADSC=1
			while( ((ADCSRA&0x10) == 0x00) ); // wait for ADC complete

			ADC_DATA[1] = ADC;
			
			PORTA = 0x00;

			ADMUX = 0xC2; // prepare for ADC_2

			break;

			////////////////////////////////////////////////////////////////////////////////////
			//------------------------------------ADC_2---------------------------------------//
			////////////////////////////////////////////////////////////////////////////////////
			case 2 :
			PORTA = 0x04;

			ADCSRA |= 0xC0; // ADEN=1, ADSC=1
			while( ((ADCSRA&0x10) == 0x00) ); // wait for ADC complete

			ADC_DATA[2] = ADC;
			
			PORTA = 0x00;

			ADMUX = 0xC3; // prepare for ADC_3

			break;

			////////////////////////////////////////////////////////////////////////////////////
			//------------------------------------ADC_3---------------------------------------//
			////////////////////////////////////////////////////////////////////////////////////
			case 3 :
			PORTA = 0x08;

			ADCSRA |= 0xC0; // ADEN=1, ADSC=1
			while( ((ADCSRA&0x10) == 0x00) ); // wait for ADC complete

			ADC_DATA[3] = ADC;
			
			PORTA = 0x00;

			ADMUX = 0xC4; // prepare for ADC_4

			break;

			////////////////////////////////////////////////////////////////////////////////////
			//------------------------------------ADC_4---------------------------------------//
			////////////////////////////////////////////////////////////////////////////////////
			case 4 :

			PORTA = 0x10;

			ADCSRA |= 0xC0; // ADEN=1, ADSC=1
			while( ((ADCSRA&0x10) == 0x00) ); // wait for ADC complete

			ADC_DATA[4] = ADC;
			
			PORTA = 0x00;

			ADMUX = 0xC5; // prepare for ADC_5

			break;

			

			////////////////////////////////////////////////////////////////////////////////////
			//------------------------------------ADC_5---------------------------------------//
			////////////////////////////////////////////////////////////////////////////////////
			case 5 :
			PORTA = 0x20;

			ADCSRA |= 0xC0; // ADEN=1, ADSC=1
			while( ((ADCSRA&0x10) == 0x00) ); // wait for ADC complete

			ADC_DATA[5] = ADC;
			
			PORTA = 0x00;

			ADMUX = 0xC6; // prepare for ADC_6

			break;



			////////////////////////////////////////////////////////////////////////////////////
			//------------------------------------ADC_6---------------------------------------//
			////////////////////////////////////////////////////////////////////////////////////
			case 6 :
			PORTA = 0x40;

			ADCSRA |= 0xC0; // ADEN=1, ADSC=1
			while( ((ADCSRA&0x10) == 0x00) ); // wait for ADC complete

			ADC_DATA[6] = ADC;
			
			PORTA = 0x00;

			ADMUX = 0xC7; // prepare for ADC_7

			break;

			

			////////////////////////////////////////////////////////////////////////////////////
			//------------------------------------ADC_7---------------------------------------//
			////////////////////////////////////////////////////////////////////////////////////
			case 7 :
			PORTA = 0x80;

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
void ADC_MAX_MIN(int *ADC_MAX, int *ADC_MIN, const int *ADC_DATA) {

	int i=0;

	for(i=IR_NUMBER_m ; i<=IR_NUMBER_M ; i++) {
		
		if( ADC_MAX[i] < ADC_DATA[i] ) // ADC_MAX
		ADC_MAX[i] = ADC_DATA[i];
		
		if( ADC_MIN[i] > ADC_DATA[i]) // ADC_MIN
		ADC_MIN[i] = ADC_DATA[i];
		
	}
}


// Get normalized ADC value (white base / black line / resolution=100)
void ADC_NORMALIZE(int *NORM_DATA, const int *ADC_DATA, const int *ADC_MAX, const int *ADC_MIN) {
	
	int i=0;
	double temp_1=0, temp_2=0;

	for(i=IR_NUMBER_m ; i<=IR_NUMBER_M ; i++) {

		temp_1 = ADC_MAX[i] - ADC_DATA[i];
		temp_2 = ADC_MAX[i] - ADC_MIN[i];

		NORM_DATA[i] = (int)((RESOLUTION*temp_1)/temp_2);

	}

}

void DATA_OUT(const int *NORM_DATA) {

	int i=0;
	int k=0x01;
	int p=0;

	for(i=IR_NUMBER_m ; i<=IR_NUMBER_M ; i++) {
		
		p=k<<i;

		if(NORM_DATA[i]>=(int)(RESOLUTION*0.6)) {
			PORTA = PORTA&(~p);
		}
		else {
			PORTA = PORTA|p;
		}

	}
}
