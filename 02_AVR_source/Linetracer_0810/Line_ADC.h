#define IR_NUMBER_m 0
#define IR_NUMBER_M 8
#define DELAY 8
#define RESOLUTION 100
volatile int state = 0, i=0, j=0;

void ADC_get(void) {

	//------------IR order-----------//	
	// 1 - 2 - 3 - 4 - 5 - 6 - 7 - 8 //

	for(i=IR_NUMBER_m ; i<IR_NUMBER_M ; i++) {			
		
		switch(i) {
			
			////////////////////////////////////////////////////////////////////////////////////
			//------------------------------------ADC_1---------------------------------------//
			////////////////////////////////////////////////////////////////////////////////////
			case 0 :
				PORTB = 0x01;

				_delay_us(DELAY);

				ADCSRA |= 0xC0; // ADEN=1, ADSC=1
				while( ((ADCSRA&0x10) == 0x00) ); // wait for ADC complete
				ADC_DATA[0] = ADC;
				
				PORTB = 0x00;
				
				ADMUX = 0x01; // prepare for ADC_1

			break;
			
			////////////////////////////////////////////////////////////////////////////////////
			//------------------------------------ADC_2---------------------------------------//
			////////////////////////////////////////////////////////////////////////////////////
			case 1 :
				PORTB = 0x02;
				
				_delay_us(DELAY);

				ADCSRA |= 0xC0; // ADEN=1, ADSC=1
				while( ((ADCSRA&0x10) == 0x00) ); // wait for ADC complete
				ADC_DATA[4] = ADC;
				
				PORTB = 0x00;
				
				ADMUX = 0x02; // prepare for ADC_2

			break;
			
			////////////////////////////////////////////////////////////////////////////////////
			//------------------------------------ADC_3---------------------------------------//
			////////////////////////////////////////////////////////////////////////////////////
			case 2 :
				PORTB = 0x04;
				
				_delay_us(DELAY);

				ADCSRA |= 0xC0; // ADEN=1, ADSC=1
				while( ((ADCSRA&0x10) == 0x00) ); // wait for ADC complete
				ADC_DATA[1] = ADC;
				
				PORTB = 0x00;
				
				ADMUX = 0x03; // prepare for ADC_3

			break;
			
			////////////////////////////////////////////////////////////////////////////////////
			//------------------------------------ADC_4---------------------------------------//
			////////////////////////////////////////////////////////////////////////////////////
			case 3 :
				PORTB = 0x08;
				
				_delay_us(DELAY);

				ADCSRA |= 0xC0; // ADEN=1, ADSC=1
				while( ((ADCSRA&0x10) == 0x00) ); // wait for ADC complete
				ADC_DATA[5] = ADC;
				
				PORTB = 0x00;
				
				ADMUX = 0x04; // prepare for ADC_4

			break;
			
			////////////////////////////////////////////////////////////////////////////////////
			//------------------------------------ADC_5---------------------------------------//
			////////////////////////////////////////////////////////////////////////////////////
			case 4 :
				PORTB = 0x10;
				
				_delay_us(DELAY);

				ADCSRA |= 0xC0; // ADEN=1, ADSC=1
				while( ((ADCSRA&0x10) == 0x00) ); // wait for ADC complete
				ADC_DATA[2] = ADC;
				
				PORTB = 0x00;
				
				ADMUX = 0x05; // prepare for ADC_5

			break;
			
			////////////////////////////////////////////////////////////////////////////////////
			//------------------------------------ADC_6---------------------------------------//
			////////////////////////////////////////////////////////////////////////////////////
			case 5 :
				PORTB = 0x20;
				
				_delay_us(DELAY);
				ADCSRA |= 0xC0; // ADEN=1, ADSC=1
				while( ((ADCSRA&0x10) == 0x00) ); // wait for ADC complete
				ADC_DATA[6] = ADC;
				
				PORTB = 0x00;
				
				ADMUX = 0x07; // prepare for ADC_7

			break;
			
			////////////////////////////////////////////////////////////////////////////////////
			//------------------------------------ADC_7---------------------------------------//
			////////////////////////////////////////////////////////////////////////////////////
			case 6 :
				PORTB = 0x40;
				
				_delay_us(DELAY);

				ADCSRA |= 0xC0; // ADEN=1, ADSC=1
				while( ((ADCSRA&0x10) == 0x00) ); // wait for ADC complete
				ADC_DATA[3] = ADC;
				
				PORTB = 0x00;
				
				ADMUX = 0x08; // prepare for ADC_8

			break;
			
			////////////////////////////////////////////////////////////////////////////////////
			//------------------------------------ADC_8---------------------------------------//
			////////////////////////////////////////////////////////////////////////////////////
			case 7 :
				PORTB = 0x80;
				
				_delay_us(DELAY);

				ADCSRA |= 0xC0; // ADEN=1, ADSC=1
				while( ((ADCSRA&0x10) == 0x00) ); // wait for ADC complete
				ADC_DATA[7] = ADC;
				
				PORTB = 0x00;
				
				ADMUX = 0x00; // prepare for ADC_0

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

	}

}

// Get Weighted value for normalize data(NORM_DATA[i])
void ADC_weight(void) {

	int weight[8] = {100, 70, 30, 10, -10, -30, -70, -100};
	
	for(i=IR_NUMBER_m ; i<IR_NUMBER_M ; i++) {

		WEIGHT_DATA[i] = NORM_DATA[i] * weight[i];
	
	}

}

// Get sensor_data from weighted data(WEIGHT_DATA[i])
void Sensor_data(void) {

	int count_sensor=0, temp=0;

	// count the number of sensor determined that it is sensed
	for(i=IR_NUMBER_m ; i<IR_NUMBER_M ; i++) {
		
		if(NORM_DATA[i] >= 50) count_sensor++;
		// 50 is a min value that can be thought that it is sensed

	}


	for(i=IR_NUMBER_m ; i<IR_NUMBER_M ; i++) {
		
		if(NORM_DATA[i] >= 50) {
			temp += WEIGHT_DATA[i]; // the sum of weighted data for finding error
		}

	}

	if(count_sensor == 0) 
		SENSOR_DATA = 0;
	else 
		SENSOR_DATA = (float)(temp / count_sensor);

}
