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
#define RESOLUTION 50

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
volatile double SENSOR_DATA = 0;

volatile double Cruze_speed = 20.0;

volatile int L_CNT = 0;
volatile int R_CNT = 0;
volatile int LM_speed = 0;
volatile int RM_speed = 0;

volatile double PD_Target	= 0.0;
volatile double Err		  	= 0.0;
volatile double Pre_Err		= 0.0;
volatile double kp			= 15.0;
volatile double kd			= 2.0;
volatile double PD_OUT		= 0.0;

volatile double L_PI_Target		= 0.0;
volatile double L_Err		  	= 0.0;
volatile double L_Vp			= 15.0;
volatile double L_PI_OUT		= 0.0;
volatile double Pre_L_PI_OUT = 0.0;

volatile double R_PI_Target		= 0.0;
volatile double R_Err		  	= 0.0;
volatile double R_Vp			= 15.0;
volatile double R_PI_OUT		= 0.0;
volatile double Pre_R_PI_OUT 	= 0.0;

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
SIGNAL(INT4_vect)
{
	cli();
	if(PINE&0x10)
	{
		if(PINE&0x20)	
			L_CNT--;
		else
			L_CNT++;
	}
	else
	{
		if(PINE&0x20)	
			L_CNT++;
		else
			L_CNT--;
	}
	sei();
}
SIGNAL(INT5_vect)
{
	cli();
	if(PINE&0x20)
	{
		if(PINE&0x10)	
			L_CNT++;
		else
			L_CNT--;
	}
	else
	{
		if(PINE&0x10)	
			L_CNT--;
		else
			L_CNT++;
	}
	sei();
}


SIGNAL(INT6_vect)
{
	cli();
	if(PINE&0x40)
	{
		if(PINE&0x80)	
			R_CNT++;
		else
			R_CNT--;
	}
	else
	{
		if(PINE&0x80)	
			R_CNT--;
		else
			R_CNT++;
	}
	sei();
}
SIGNAL(INT7_vect)
{
	cli();
	if(PINE&0x80)
	{
		if(PINE&0x40)	
			R_CNT--;
		else
			R_CNT++;
	}
	else
	{
		if(PINE&0x40)	
			R_CNT++;
		else
			R_CNT--;
	}
	sei();
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

				ADC_max_min();

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

				ADC_get();

				ADC_normalize();

				ADC_weight();

				Sensor_data();
				/*
				//----- 	 PD control 	  -----//
				Err = SENSOR_DATA;
				PD_OUT = ((kp*Err)+(kd*(Pre_Err - Err)))
				*/

				if(SENSOR_DATA >= 0 ) {
					L_PI_Target = Cruze_speed + (SENSOR_DATA/400.0);
					R_PI_Target = Cruze_speed - (SENSOR_DATA/400.0);
				}
				else {
					R_PI_Target = Cruze_speed + (SENSOR_DATA/400.0);
					L_PI_Target = Cruze_speed - (SENSOR_DATA/400.0);
				}
				if(L_PI_Target>=100)
					L_PI_Target = 100;
				
				if(R_PI_Target>=100)
					R_PI_Target = 100;

				if(L_PI_Target<=0)
					L_PI_Target = 0;
				
				if(R_PI_Target<=0)
					R_PI_Target = 0;

				//----- Left_Motor PI control -----//
				L_Err 			= L_PI_Target - L_CNT; // Error = 목표값 - 현재값
				L_PI_OUT		= L_Vp * L_Err + Pre_L_PI_OUT; // PI_out = P*Err + I*(Err sum)
				
				//----- Right_Motor PI control -----//
				R_Err 			= R_PI_Target - R_CNT;
				R_PI_OUT		= R_Vp * R_Err + Pre_R_PI_OUT;

				MOTOR_speed(L_PI_OUT,R_PI_OUT);

				L_CNT = 0;
				R_CNT = 0;
				Pre_L_PI_OUT = L_PI_OUT;
				Pre_R_PI_OUT = R_PI_OUT;

			break;
	}

	sei();
}

// Get ADC from IR sensor
void ADC_get(void) {

	//------------IR order-----------//	
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
				
				ADMUX = 0x04; // prepare for ADC_4

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
				
				ADMUX = 0x01; // prepare for ADC_1

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
				
				ADMUX = 0x05; // prepare for ADC_5

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
				
				ADMUX = 0x02; // prepare for ADC_2

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
				
				ADMUX = 0x06; // prepare for ADC_6

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
				
				ADMUX = 0x03; // prepare for ADC_3

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
				
				ADMUX = 0x07; // prepare for ADC_7

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

void MOTOR_speed(int lm_speed, int rm_speed) {	
	
	// because the maximum value of OCR1A and OCR1B is 16000
	if(lm_speed > 15500) lm_speed = 16000;
	
	else if(lm_speed < -15500) lm_speed = -16000;

	if(rm_speed > 15500) rm_speed = 16000;
	
	else if(rm_speed < -15500) rm_speed = -16000;


	if(lm_speed >= 0) {
		sbi(PORTC, PORTC2); // nomal direction
		OCR1A = lm_speed;
	}
	else if(lm_speed < 0) {
		cbi(PORTC, PORTC2); // reverse direction
		OCR1A = -1 * lm_speed;
	}
	
	if(rm_speed >= 0) {
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
