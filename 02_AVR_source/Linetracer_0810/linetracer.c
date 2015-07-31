#include<avr/io.h>
#include<util/delay.h>
#include<avr/interrupt.h>
#include"MCU_init.h"

#define IDLE 0 // 대기모드
#define NORM 1 // 측정모드
#define WAIT 2 // 대기모드
#define RUN 3 // 작동모드

#define IR_NUMBER_m 0
#define IR_NUMBER_M 8
#define DELAY 8
#define RESOLUTION 100

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

volatile double Cruze_speed = 4000.0;

volatile int L_CNT = 0;
volatile int R_CNT = 0;
volatile int LM_speed = 0;
volatile int RM_speed = 0;

volatile double PD_Target	= 0.0;
volatile double Err		  	= 0.0;
volatile double Pre_Err		= 0.0;
volatile double kp			= 5.0;
volatile double ki			= 0.1;
volatile double kd			= 0.05;
volatile double PD_OUT		= 0.0;

volatile double L_PI_Target		= 0.0;
volatile double L_Err		  	= 0.0;
volatile double L_Vp			= 5.0;
volatile double L_PI_OUT		= 0.0;
volatile double Pre_L_PI_OUT = 0.0;

volatile double R_PI_Target		= 0.0;
volatile double R_Err		  	= 0.0;
volatile double R_Vp			= 5.0;
volatile double R_PI_OUT		= 0.0;
volatile double Pre_R_PI_OUT 	= 0.0;

int END_FLAG=0;
int CNT=0;

void ADC_get(void);
void ADC_max_min(void);
void ADC_normalize(void);
void ADC_weight(void);
void ADC_uart(void);
void Sensor_data(void);
void ADC_state(void);

void MOTOR_speed(int lm_speed, int rm_speed);
void MOTOR_control(void);

int main(void) {

	init_PORT();
	init_TIMER0();
	init_TIMER();
	init_EX_INTERRUPT();
	init_ADC();
	init_USART();
	init_PWM();
	
	sei();
	
	while(1) {
		if(state>3) state = 3;
	}
	
	return 0;
}

// State switch
SIGNAL(INT0_vect) {

	cli();
	ADC_state();
	state++;
	
	sei();
}
/*
SiGNAL(SIG_OVERFLOW0) {
	
	CNT++;

	if(CNT > 240) {
		END_FLAG = 1;
		CNT = 0;
		TIMSK &= 0xFE;

	}

}
*/
/*
SIGNAL(INT6_vect)
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
SIGNAL(INT7_vect)
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


SIGNAL(INT4_vect)
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
SIGNAL(INT5_vect)
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
*/
// State / 20ms control period
SIGNAL(SIG_OVERFLOW3) {
	
	cli();
	/*
	CNT++;
	if (CNT == 3000){
		state = WAIT;
	}
	*/
	switch(state) {
			////////////////////////////////////////////////////////////////////////////////////
			//-------------------------------------IDLE---------------------------------------//
			////////////////////////////////////////////////////////////////////////////////////
			case IDLE :
				PORTD = 0x14; // IDLE state check
			break;
			
			
			////////////////////////////////////////////////////////////////////////////////////
			//-------------------------------------NORM---------------------------------------//
			////////////////////////////////////////////////////////////////////////////////////
			case NORM :
				PORTD = 0x10; // NORM state check
				ADC_get();

				ADC_max_min();

	
				ADC_uart();
				

			break;
			
			////////////////////////////////////////////////////////////////////////////////////
			//-------------------------------------WAIT---------------------------------------//
			////////////////////////////////////////////////////////////////////////////////////
			case WAIT :
				PORTD = 0x04; // WAIT check

			break;
			
			////////////////////////////////////////////////////////////////////////////////////
			//-------------------------------------RUN----------------------------------------//
			////////////////////////////////////////////////////////////////////////////////////
			case RUN :
				PORTD = 0x00; // RUN check

				ADC_get();
				/*
				if((ADC_DATA[0]>=(ADC_MAX[0]/2)) && (ADC_DATA[7]>=(ADC_MAX[0]/2))){
					while(1){PORTC=0x00;}
				}
				*/
				ADC_normalize();

				ADC_weight();
/*
				if(WEIGHT_DATA[2]<=ADC_MIN[2] && WEIGHT_DATA[5]<=ADC_MIN[5] && WEIGHT_DATA[0]  && WEIGHT_DATA[7] != 0) {
					TIMSK = 0x01;

					if( END_FLAG == 1) state = IDLE;
				}
*/
				Sensor_data();

				MOTOR_control();


				/*
				//----- 	 PD control 	  -----//
				Err = SENSOR_DATA;
				PD_OUT = ((kp*Err)+(kd*(Pre_Err - Err)));
				

				if(SENSOR_DATA >= 0 ) {
					L_PI_Target = Cruze_speed + (PD_OUT/400.0);
					R_PI_Target = Cruze_speed - (PD_OUT/400.0);
				}
				else {
					R_PI_Target = Cruze_speed + (PD_OUT/400.0);
					L_PI_Target = Cruze_speed - (PD_OUT/400.0);
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
				L_PI_OUT		= L_Vp * L_Err + (ki*(Pre_L_PI_OUT)); // PI_out = P*Err + I*(Err sum)
				
				//----- Right_Motor PI control -----//
				R_Err 			= R_PI_Target - R_CNT;
				R_PI_OUT		= R_Vp * R_Err + (ki*(Pre_L_PI_OUT));
				
				MOTOR_speed(L_PI_Target,R_PI_Target);

				L_CNT = 0;
				R_CNT = 0;
				Pre_L_PI_OUT = L_PI_OUT;
				Pre_R_PI_OUT = R_PI_OUT;
				*/

				ADC_uart();
			break;
	}

	sei();
}

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
				
				PORTA = 0x01;

				_delay_us(DELAY);

				ADCSRA |= 0xC0; // ADEN=1, ADSC=1
				while(!(ADCSRA&0x10)); // wait for ADC complete
				ADC_DATA[0] = ADCL;
				ADC_DATA[0] += (ADCH << 8);
				
				PORTA = 0x00;
				
				ADMUX = 0xC1; // prepare for ADC_2

			break;
			
			////////////////////////////////////////////////////////////////////////////////////
			//------------------------------------ADC_2---------------------------------------//
			////////////////////////////////////////////////////////////////////////////////////
			case 1 :
				PORTA = 0x02;
				
				_delay_us(DELAY);

				ADCSRA |= 0xC0; // ADEN=1, ADSC=1
				while(!(ADCSRA&0x10)); // wait for ADC complete
				ADC_DATA[1] = ADCL;
				ADC_DATA[1] += (ADCH << 8);
				
				PORTA = 0x00;
				
				ADMUX = 0xC2; // prepare for ADC_3

			break;
			
			////////////////////////////////////////////////////////////////////////////////////
			//------------------------------------ADC_3---------------------------------------//
			////////////////////////////////////////////////////////////////////////////////////
			case 2 :
				PORTA = 0x04;
				
				_delay_us(DELAY);

				ADCSRA |= 0xC0; // ADEN=1, ADSC=1
				while(!(ADCSRA&0x10)); // wait for ADC complete
				ADC_DATA[2] = ADCL;
				ADC_DATA[2] += (ADCH << 8);
				
				PORTA = 0x00;
				
				ADMUX = 0xC3; // prepare for ADC_4

			break;
			
			////////////////////////////////////////////////////////////////////////////////////
			//------------------------------------ADC_4---------------------------------------//
			////////////////////////////////////////////////////////////////////////////////////
			case 3 :
				PORTA = 0x08;
				
				_delay_us(DELAY);

				ADCSRA |= 0xC0; // ADEN=1, ADSC=1
				while(!(ADCSRA&0x10)); // wait for ADC complete
				ADC_DATA[3] = ADCL;
				ADC_DATA[3] += (ADCH << 8);
				
				
				PORTA = 0x00;
				
				ADMUX = 0xC4; // prepare for ADC_5

			break;
			
			////////////////////////////////////////////////////////////////////////////////////
			//------------------------------------ADC_5---------------------------------------//
			////////////////////////////////////////////////////////////////////////////////////
			case 4 :
				PORTA = 0x10;
				
				_delay_us(DELAY);

				ADCSRA |= 0xC0; // ADEN=1, ADSC=1
				while(!(ADCSRA&0x10)); // wait for ADC complete
				ADC_DATA[4] = ADCL;
				ADC_DATA[4] += (ADCH << 8);
				
				
				PORTA = 0x00;
				
				ADMUX = 0xC5; // prepare for ADC_6

			break;
			
			////////////////////////////////////////////////////////////////////////////////////
			//------------------------------------ADC_6---------------------------------------//
			////////////////////////////////////////////////////////////////////////////////////
			case 5 :
				PORTA = 0x20;
				
				_delay_us(DELAY);

				ADCSRA |= 0xC0; // ADEN=1, ADSC=1
				while(!(ADCSRA&0x10)); // wait for ADC complete
				ADC_DATA[5] = ADCL;
				ADC_DATA[5] += (ADCH << 8);
				
				PORTA = 0x00;
				
				ADMUX = 0xC6; // prepare for ADC_7

			break;
			
			////////////////////////////////////////////////////////////////////////////////////
			//------------------------------------ADC_7---------------------------------------//
			////////////////////////////////////////////////////////////////////////////////////
			case 6 :
				PORTA = 0x40;
				
				_delay_us(DELAY);

				ADCSRA |= 0xC0; // ADEN=1, ADSC=1
				while(!(ADCSRA&0x10)); // wait for ADC complete
				ADC_DATA[6] = ADCL;
				ADC_DATA[6] += (ADCH << 8);
				
				
				PORTA = 0x00;
				
				ADMUX = 0xC7; // prepare for ADC_8

			break;
			
			////////////////////////////////////////////////////////////////////////////////////
			//------------------------------------ADC_8---------------------------------------//
			////////////////////////////////////////////////////////////////////////////////////
			case 7 :
				PORTA = 0x80;
				
				_delay_us(DELAY);

				ADCSRA |= 0xC0; // ADEN=1, ADSC=1
				while(!(ADCSRA&0x10)); // wait for ADC complete
				ADC_DATA[7] = ADCL;
				ADC_DATA[7] += (ADCH << 8);
				
				
				PORTA = 0x00;

				ADMUX = 0xC0; // prepare for ADC_1

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

	int weight[8] = {90, 75, 40, 10, -10, -40, -75, -90};
	
	for(i=IR_NUMBER_m ; i<IR_NUMBER_M ; i++) {

		WEIGHT_DATA[i] = NORM_DATA[i] * weight[i];
	
	}

}

// Get sensor_data from weighted data(WEIGHT_DATA[i])
void Sensor_data(void) {

	int count_sensor=0, temp=0;

	// count the number of sensor determined that it is sensed
	for(i=IR_NUMBER_m ; i<IR_NUMBER_M ; i++) {
		
		if(NORM_DATA[i] >= 60) count_sensor++;
		// 50 is a min value that can be thought that it is sensed

	}


	for(i=IR_NUMBER_m ; i<IR_NUMBER_M ; i++) {
		
		if(NORM_DATA[i] >= 60) {
			temp += WEIGHT_DATA[i]; // the sum of weighted data for finding error
		}

	}

	if(count_sensor == 0) 
		SENSOR_DATA = 0;
	else 
		SENSOR_DATA = (float)(temp / count_sensor);

}

void MOTOR_Lspeed(int lm_speed, int rm_speed) {	
	
	// because the maximum value of OCR1A and OCR1B is 16000
/*
	if(lm_speed > 15500) lm_speed = 16000;
	
	else if(lm_speed < -15500) lm_speed = -16000;

	if(rm_speed > 15500) rm_speed = 16000;
	
	else if(rm_speed < -15500) rm_speed = -16000;

	if(lm_speed >= 0) {
		cbi(PORTC, PORTC3); // nomal direction
		OCR1A = lm_speed;
	}
	else if(lm_speed < 0) {
		sbi(PORTC, PORTC3); // reverse direction
		OCR1A = -1 * lm_speed;
	}
	
	if(rm_speed >= 0) {
		sbi(PORTC, PORTC5); // nomal direction
		OCR1B = rm_speed;
	}
	else if(rm_speed < 0 ) {
		cbi(PORTC, PORTC5); // reverse direction
		OCR1B = -1 * rm_speed;
	}


	if(rm_speed >= 0) {
		sbi(PORTC, PORTC5); // nomal direction
		cbi(PORTC, PORTC6);
		OCR1B =  0.1 * rm_speed;
	}
	else if(rm_speed < 0 ) {
		sbi(PORTC, PORTC5); // nomal direction
		cbi(PORTC, PORTC6);
		OCR1B = (-0.1) * rm_speed;
	}
*/
	if(lm_speed >= 0) {
		cbi(PORTC, PORTC4); // nomal direction
		OCR1A = 0.18 * lm_speed;
	}
	else if(lm_speed < 0) {
		cbi(PORTC, PORTC4); // nomal direction
		OCR1A = (-0.18) * lm_speed;
	}


	
}
void MOTOR_Rspeed(int lm_speed, int rm_speed) {	

	if(rm_speed >= 0) {
		sbi(PORTC, PORTC5); // nomal direction
		cbi(PORTC, PORTC6);
		OCR1B =  0.18 * rm_speed;
	}
	else if(rm_speed < 0 ) {
		sbi(PORTC, PORTC5); // nomal direction
		cbi(PORTC, PORTC6);
		OCR1B = (-0.18) * rm_speed;
	}
	}
void MOTOR_control(void) {
	
	if(SENSOR_DATA > 0 ) {
		MOTOR_Lspeed(Cruze_speed - SENSOR_DATA, Cruze_speed + SENSOR_DATA);
		MOTOR_Rspeed(Cruze_speed - SENSOR_DATA, Cruze_speed + SENSOR_DATA);
	}
	else if(SENSOR_DATA < 0) {
		MOTOR_Lspeed(Cruze_speed + (-1*SENSOR_DATA), Cruze_speed + SENSOR_DATA);
		MOTOR_Rspeed(Cruze_speed + (-1*SENSOR_DATA), Cruze_speed + SENSOR_DATA);
	}

}

void ADC_state(void) {
	Uart_Putch0('s');
	Uart_U16Bit_PutNum0(state);
}
void ADC_uart(void) {
/*	
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
	*/
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
/*
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
*/
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
/*
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
*/
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

	Uart_Putch0('\t');

	//---------------MOTOR_speed output---------------//
	Uart_Putch0('M');
	Uart_Putch0('o');
	Uart_Putch0('t');
	Uart_Putch0('\t');

	Uart_U16Bit_PutNum0(OCR1B);


	Uart_Putch0('\n');
	Uart_Putch0('\r');

	Uart_Putch0('\n');
	Uart_Putch0('\r');

}
