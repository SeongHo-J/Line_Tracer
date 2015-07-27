#include"linetracer_ADC.h"

// Get ADC from IR sensor
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
				PORTD = 0x01;

				_delay_us(DELAY);

				ADCSRA |= 0xC0; // ADEN=1, ADSC=1
				while( ((ADCSRA&0x10) == 0x00) ); // wait for ADC complete

				ADC_DATA[0] = ADC;
				
				PORTD = 0x00;

				ADMUX = 0xC1; // prepare for ADC_1

			break;

			////////////////////////////////////////////////////////////////////////////////////
			//------------------------------------ADC_1---------------------------------------//
			////////////////////////////////////////////////////////////////////////////////////
			case 1 :

				PORTD = 0x02;

				_delay_us(DELAY);

				ADCSRA |= 0xC0; // ADEN=1, ADSC=1
				while( ((ADCSRA&0x10) == 0x00) ); // wait for ADC complete

				ADC_DATA[1] = ADC;
				
				PORTD = 0x00;

				ADMUX = 0xC2; // prepare for ADC_2

			break;

			////////////////////////////////////////////////////////////////////////////////////
			//------------------------------------ADC_2---------------------------------------//
			////////////////////////////////////////////////////////////////////////////////////
			case 2 :
				PORTD = 0x04;

				_delay_us(DELAY);

				ADCSRA |= 0xC0; // ADEN=1, ADSC=1
				while( ((ADCSRA&0x10) == 0x00) ); // wait for ADC complete

				ADC_DATA[2] = ADC;
				
				PORTD = 0x00;

				ADMUX = 0xC3; // prepare for ADC_3

			break;

			////////////////////////////////////////////////////////////////////////////////////
			//------------------------------------ADC_3---------------------------------------//
			////////////////////////////////////////////////////////////////////////////////////
			case 3 :
				PORTD = 0x08;

				_delay_us(DELAY);

				ADCSRA |= 0xC0; // ADEN=1, ADSC=1
				while( ((ADCSRA&0x10) == 0x00) ); // wait for ADC complete

				ADC_DATA[3] = ADC;
				
				PORTD = 0x00;

				ADMUX = 0xC4; // prepare for ADC_4

			break;

			////////////////////////////////////////////////////////////////////////////////////
			//------------------------------------ADC_4---------------------------------------//
			////////////////////////////////////////////////////////////////////////////////////
			case 4 :

				PORTD = 0x10;

				_delay_us(DELAY);

				ADCSRA |= 0xC0; // ADEN=1, ADSC=1
				while( ((ADCSRA&0x10) == 0x00) ); // wait for ADC complete

				ADC_DATA[4] = ADC;
				
				PORTD = 0x00;

				ADMUX = 0xC5; // prepare for ADC_5

			break;

			

			////////////////////////////////////////////////////////////////////////////////////
			//------------------------------------ADC_5---------------------------------------//
			////////////////////////////////////////////////////////////////////////////////////
			case 5 :
				PORTD = 0x20;

				_delay_us(DELAY);

				ADCSRA |= 0xC0; // ADEN=1, ADSC=1
				while( ((ADCSRA&0x10) == 0x00) ); // wait for ADC complete

				ADC_DATA[5] = ADC;
				
				PORTD = 0x00;

				ADMUX = 0xC6; // prepare for ADC_6

			break;



			////////////////////////////////////////////////////////////////////////////////////
			//------------------------------------ADC_6---------------------------------------//
			////////////////////////////////////////////////////////////////////////////////////
			case 6 :
				PORTD = 0x40;

				_delay_us(DELAY);

				ADCSRA |= 0xC0; // ADEN=1, ADSC=1
				while( ((ADCSRA&0x10) == 0x00) ); // wait for ADC complete

				ADC_DATA[6] = ADC;
				
				PORTD = 0x00;

				ADMUX = 0xC7; // prepare for ADC_7

			break;

			

			////////////////////////////////////////////////////////////////////////////////////
			//------------------------------------ADC_7---------------------------------------//
			////////////////////////////////////////////////////////////////////////////////////
			case 7 :
				PORTD = 0x80;

				_delay_us(DELAY);

				ADCSRA |= 0xC0; // ADEN=1, ADSC=1
				while( ((ADCSRA&0x10) == 0x00) ); // wait for ADC complete

				ADC_DATA[7] = ADC;
				
				PORTD = 0x00;
				
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

void DATA_WEIGHT(int *DIRECTION, int *WEIGHT_DATA, const int *NORM_DATA) {

	int i=0;
	int cnt=0;

	//int weight[8] = {-100, -125, -150, -200, 200, 150, 125, 100};
	int weight[8] = {-80, -70, -50, 0, 0, 50, 70, 80};

	for(i=IR_NUMBER_m ; i<=IR_NUMBER_M ; i++) {

		if(NORM_DATA[i]>=(int)(RESOLUTION*0.6)) {

			WEIGHT_DATA[i] = weight[i];
			*DIRECTION = *DIRECTION + weight[i];
			cnt++;

		}
		else {

			WEIGHT_DATA[i] = 0;

		}

	}

	if(cnt!=0) *DIRECTION = (int)(*DIRECTION/cnt);

}
