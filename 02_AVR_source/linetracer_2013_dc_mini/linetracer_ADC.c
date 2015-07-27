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
				PORTD |= 0x10;

				_delay_us(DELAY);

				ADCSRA |= 0xC0; // ADEN=1, ADSC=1
				while( ((ADCSRA&0x10) == 0x00) ); // wait for ADC complete

				ADC_DATA[0] = ADC;
				
				PORTD &= ~(0x10);

				ADMUX = 0xC1; // prepare for ADC_1

			break;

			////////////////////////////////////////////////////////////////////////////////////
			//------------------------------------ADC_1---------------------------------------//
			////////////////////////////////////////////////////////////////////////////////////
			case 1 :

				PORTD |= 0x20;

				_delay_us(DELAY);

				ADCSRA |= 0xC0; // ADEN=1, ADSC=1
				while( ((ADCSRA&0x10) == 0x00) ); // wait for ADC complete

				ADC_DATA[1] = ADC;
				
				PORTD &= ~(0x20);

				ADMUX = 0xC2; // prepare for ADC_2

			break;

			////////////////////////////////////////////////////////////////////////////////////
			//------------------------------------ADC_2---------------------------------------//
			////////////////////////////////////////////////////////////////////////////////////
			case 2 :
				PORTD |= 0x40;

				_delay_us(DELAY);

				ADCSRA |= 0xC0; // ADEN=1, ADSC=1
				while( ((ADCSRA&0x10) == 0x00) ); // wait for ADC complete

				ADC_DATA[2] = ADC;
				
				PORTD &= ~(0x40);

				ADMUX = 0xC3; // prepare for ADC_3

			break;

			////////////////////////////////////////////////////////////////////////////////////
			//------------------------------------ADC_3---------------------------------------//
			////////////////////////////////////////////////////////////////////////////////////
			case 3 :
				PORTD |= 0x80;

				_delay_us(DELAY);

				ADCSRA |= 0xC0; // ADEN=1, ADSC=1
				while( ((ADCSRA&0x10) == 0x00) ); // wait for ADC complete

				ADC_DATA[3] = ADC;
				
				PORTD &= ~(0x80);

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

void DATA_WEIGHT(int *SENSOR_DATA, int *WEIGHT_DATA, const int *NORM_DATA) {

	int i=0;
	int cnt=0;
	int weight[4] = {-3, -1, 1, 3};

	for(i=IR_NUMBER_m ; i<=IR_NUMBER_M ; i++) {

		if(NORM_DATA[i]>=(int)(RESOLUTION*0.6)) {

			WEIGHT_DATA[i] = weight[i]*NORM_DATA[i];
			*SENSOR_DATA = *SENSOR_DATA + WEIGHT_DATA[i];
			cnt++;

		}

	}

	if(cnt!=0) *SENSOR_DATA = (int)(*SENSOR_DATA/cnt);
	else *SENSOR_DATA=0;

}
