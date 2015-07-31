#define IR_NUMBER_m 0
#define IR_NUMBER_M 8
volatile int state = 0, i=0, j=0;

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



void ADC_state(void) {
	Uart_Putch0('s');
	Uart_U16Bit_PutNum0(state);
}

