#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/io.h>

#define F_CPU 16000000
#ifndef _LINETRACER_ADC_H_
#define _LINETRACER_ADC_H_
#define RESOLUTION 100
#define DELAY 0
#define SPEED 0x3c
#else
#endif


void ADC_GET(int *ADC_DATA);
void ADC_MAX_MIN(int *ADC_MAX, int *ADC_MIN, const int *ADC_DATA);
void ADC_NORMALIZE(int *NORM_DATA, const int *ADC_DATA, const int *ADC_MAX, const int *ADC_MIN);
int ADC_TO_BOOL(int *NORM_DATA);
void init_PWM(void);
void init_MCU(void);
void MOTER_MOVE(int MOTER_WIGHT);
int ADC_TO_MOTER(int *NORM_DATA);

int ADC_DATA[8] = {0,};
int ADC_MAX[8] = {0,};
int ADC_MIN[8] = {1023,1023,1023,1023,1023,1023,1023,1023};
int NORM_DATA[8] = {0,};

int temp[8] = {0,};


int main(void)
{
	int temp=0;
	init_PWM();
	init_MCU();
	while(1)
	{
		ADC_GET(ADC_DATA);
		ADC_MAX_MIN(ADC_MAX,ADC_MIN,ADC_DATA);
		ADC_NORMALIZE(NORM_DATA,ADC_DATA,ADC_MAX,ADC_MIN);
		MOTER_MOVE(ADC_TO_MOTER(NORM_DATA));
	}
}
void init_MCU()
{
	DDRA = 0xff;  //신호
	DDRD = 0xff;  //발광부
	DDRE = 0b11101111;//스위치
	DDRF = 0x00;  //수신부
	PORTA = 0x08; //0b00001000
	ADMUX = 0x00;
	ADCSRA = 0x87;
	
}
void init_PWM(void)
{
	DDRB = 0xFF;
	TCCR0 = 0x6F;
	TCCR2 = 0x6D;
	TCNT0 = 0xff;
	TCNT2 = 0xff;
	OCR0 = SPEED;
	OCR2 = SPEED;
}
void ADC_GET(int *ADC_DATA)
{
	for(int i=0; i<8; i++){
		PORTD = 0x01 << i;
		_delay_us(DELAY);
		ADCSRA |= 0xC0; // ADEN = 1, ADSC = 1
		while( ((ADCSRA&0x10) == 0x00) ); // wait for ADC complete
		ADC_DATA[i] = ADC;
		PORTD = 0x00;
		if(i==7){
			ADMUX = 0xC0; // prepare for ADC_i
			}else {
			ADMUX = 0xC1+i; // prepare for ADC_i
		}
	}
}
void ADC_MAX_MIN(int *ADC_MAX, int *ADC_MIN, const int *ADC_DATA)
{
	for(int i=0 ; i<8 ; i++) {
		if( ADC_MAX[i] < ADC_DATA[i] ) // ADC_MAX
		ADC_MAX[i] = ADC_DATA[i];
		if( ADC_MIN[i] > ADC_DATA[i]) // ADC_MIN
		ADC_MIN[i] = ADC_DATA[i];
	}
}
void ADC_NORMALIZE(int *NORM_DATA, const int *ADC_DATA, const int *ADC_MAX, const int *ADC_MIN)
{
	double temp_1=0, temp_2=0;
	for(int i=0 ; i<8 ; i++) {
		temp_1 = ADC_MAX[i] - ADC_DATA[i];
		temp_2 = ADC_MAX[i] - ADC_MIN[i];
		NORM_DATA[i] = (int)(temp_1/temp_2*100);
	}
}
int ADC_TO_MOTER(int *NORM_DATA)
{
	int temp_01 = 0;
	int weight[8] = {-55 , -50 , -10 , 0 , 0 , 10 , 50 , 55};
	for(int i=0; i<8; i++){
		if(NORM_DATA[i]>=60){    //검정색
			temp_01 += weight[i];
		}
	}
	return temp_01;
}

void MOTER_MOVE(int MOTER_WIGHT){

	switch(MOTER_WIGHT){
		case -55 :
		PORTA = 0b00011000;
		break;
		case 55 :
		PORTA = 0b00101000;
		break;
		default :
		PORTA = 0b00001000;
		OCR2 = SPEED - MOTER_WIGHT;
		OCR0 = SPEED + MOTER_WIGHT;
		break;
	}
}