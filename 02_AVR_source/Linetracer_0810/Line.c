#define IR_NUMBER_m 0
#define IR_NUMBER_M 8
#define DELAY 8
#define RESOLUTION 100

// State and Usart variable
volatile int i=0, j=0;

// ADC data variable
volatile int ADC_DATA[8] = {0,};
volatile int ADC_MAX[8] = {0,};
volatile int ADC_MIN[8] = {1023,1023,1023,1023,1023,1023,1023,1023};
volatile int NORM_DATA[8] = {0,};
volatile int WEIGHT_DATA[8] = {0,};
volatile double SENSOR_DATA = 0;

// Get ADC from IR sensor
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
		
	return SENSOR_DATA;
}

void MOTOR_speed(int lm_speed, int rm_speed) {	
	
	// because the maximum value of OCR1A and OCR1B is 16000
	if(lm_speed > 15500) lm_speed = 16000;
	
	else if(lm_speed < -15500) lm_speed = -16000;

	if(rm_speed > 15500) rm_speed = 16000;
	
	else if(rm_speed < -15500) rm_speed = -16000;


	if(lm_speed >= 0) {
		sbi(PORTC, PORTC4); // nomal direction
		OCR1A = lm_speed;
	}
	else if(lm_speed < 0) {
		cbi(PORTC, PORTC4); // reverse direction
		OCR1A = -1 * lm_speed;
	}
	
	if(rm_speed >= 0) {
		cbi(PORTA, PORTA2); // nomal direction
		OCR1B = rm_speed;
	}
	else if(rm_speed < 0 ) {
		sbi(PORTA, PORTA2); // reverse direction
		OCR1B = -1 * rm_speed;
	}

}

void MOTOR_control(void) {
	
	if(SENSOR_DATA > 0 ) {
		MOTOR_speed(Cruze_speed + SENSOR_DATA, Cruze_speed - SENSOR_DATA);
	}
	else if(SENSOR_DATA < 0) {
		MOTOR_speed(Cruze_speed - SENSOR_DATA, Cruze_speed + SENSOR_DATA);
	}

}

void ADC_state(void) {
	Uart_Putch0('s');
	Uart_U16Bit_PutNum0(state);
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

	Uart_U16Bit_PutNum0(Cruze_speed-SENSOR_DATA);

	Uart_Putch0('\n');
	Uart_Putch0('\r');

	Uart_Putch0('\n');
	Uart_Putch0('\r');

}
