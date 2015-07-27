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
#define DELAY 10
#define RESOLUTION 1000

volatile int state = 0, i=0, j=0;
volatile int tho=0, hnd=0, ten=0, one=0;
volatile int ADC_DATA[8] = {0,};
volatile int ADC_MAX[8] = {0,};
volatile int ADC_MIN[8] = {1023,};
volatile int NORM_DATA[8] = {0,};
volatile double WEIGHT_DATA[8] = {0,};
volatile int A_r=0, B_r=0, A_l=0, B_l=0;

void ADC_get(void);
void ADC_max_min(void);
void ADC_normalize(void);
void ADC_weight(void);
void ADC_uart(void);
void Encoder_get(void);


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

// State / 20ms control period
SIGNAL(SIG_OVERFLOW3) {
	
	cli();

	switch(state) {
//----------------IDLE------------------//
			case IDLE :
				PORTC = 0x00; // IDLE state check

			break;
			
//----------------NORM------------------//
			case NORM :
				PORTC = 0x01; // NORM state check

				ADC_get(); // ADC get and regist ADC to ADC_DATA[i]
				//ADC_max_min(); // find ADC_DATA max value and min value
				
				ADC_uart();
			break;
			
//----------------WAIT------------------//
			case WAIT :
				PORTC = 0x02; // WAIT check

			break;

//----------------RUN-------------------//
			case RUN :
				PORTC = 0x03; // RUN check
				
				ADC_get();
				ADC_normalize(); // nomailze ADC_DATA[i]
				//ADC_weight(); // weighted NORM_DATA[i]
				
				ADC_uart();

			break;
		
	}

	sei();
}

/*SIGNAL(INT1_vect) {
	A_r++;
}
SIGNAL(INT2_vect) {
	A_l++;
}
SIGNAL(INT3_vect) {
	B_r++;
}
SIGNAL(INT6_vect) {
	B_r++;
}*/

// Get ADC from IR sensor
void ADC_get(void) {

	//-------------IR order----------//	
	// 0 - 4 - 1 - 5 - 2 - 6 - 3 - 7 //

	for(i=IR_NUMBER_m ; i<IR_NUMBER_M ; i++) {			
		
		switch(i) {
			
			/* ADC_0 convert */
			case 0 :
				PORTA = 0x01;
				
				_delay_us(DELAY);

				ADCSRA |= 0xC0; // ADEN=1, ADSC=1
				while( ((ADCSRA&0x10) == 0x00) ); // wait for ADC complete
				ADC_DATA[0] = ADC;
				
				PORTA = 0x00;
				
				ADMUX = 0xC4; // prepare for ADC_4

			break;
			
			/* ADC_4 convert */
			case 1 :
				PORTA = 0x10;
				
				_delay_us(DELAY);
				ADCSRA |= 0xC0; // ADEN=1, ADSC=1
				while( ((ADCSRA&0x10) == 0x00) ); // wait for ADC complete
				ADC_DATA[4] = ADC;
				
				PORTA = 0x00;
				
				ADMUX = 0xC1; // prepare for ADC_1

			break;
			
			/* ADC_1 convert */
			case 2 :
				PORTA = 0x02;
				
				_delay_us(DELAY);
				ADCSRA |= 0xC0; // ADEN=1, ADSC=1
				while( ((ADCSRA&0x10) == 0x00) ); // wait for ADC complete
				ADC_DATA[1] = ADC;
				
				PORTA = 0x00;
				
				ADMUX = 0xC5; // prepare for ADC_5

			break;
			
			/* ADC_5 convert */
			case 3 :
				PORTA = 0x20;
				
				_delay_us(DELAY);
				ADCSRA |= 0xC0; // ADEN=1, ADSC=1
				while( ((ADCSRA&0x10) == 0x00) ); // wait for ADC complete
				ADC_DATA[5] = ADC;
				
				PORTA = 0x00;
				
				ADMUX = 0xC2; // prepare for ADC_2

			break;
			
			/* ADC_2 convert */
			case 4 :
				PORTA = 0x04;
				
				_delay_us(DELAY);
				ADCSRA |= 0xC0; // ADEN=1, ADSC=1
				while( ((ADCSRA&0x10) == 0x00) ); // wait for ADC complete
				ADC_DATA[2] = ADC;
				
				PORTA = 0x00;
				
				ADMUX = 0xC6; // prepare for ADC_6

			break;
			
			/* ADC_6 convert */
			case 5 :
				PORTA = 0x40;
				
				_delay_us(DELAY);
				ADCSRA |= 0xC0; // ADEN=1, ADSC=1
				while( ((ADCSRA&0x10) == 0x00) ); // wait for ADC complete
				ADC_DATA[6] = ADC;
				
				PORTA = 0x00;
				
				ADMUX = 0xC3; // prepare for ADC_3

			break;
			
			/* ADC_3 convert */
			case 6 :
				PORTA = 0x08;
				
				_delay_us(DELAY);
				ADCSRA |= 0xC0; // ADEN=1, ADSC=1
				while( ((ADCSRA&0x10) == 0x00) ); // wait for ADC complete
				ADC_DATA[3] = ADC;
				
				PORTA = 0x00;
				
				ADMUX = 0xC7; // prepare for ADC_7

			break;
			
			/* ADC_7 convert */
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

// Get ADC max and min value from ADC_DATA[i]
void ADC_max_min(void) {
	
	for(i=IR_NUMBER_m ; i<IR_NUMBER_M ; i++) {					
		if( ADC_MAX[i] < ADC_DATA[i] ) // ADC_MAX
			ADC_MAX[i] = ADC_DATA[i];
		if( (ADC_MIN[i] > ADC_DATA[i]) && (0 < ADC_DATA[i]) ) // ADC_MIN
			ADC_MIN[i] = ADC_DATA[i];	
	}

}

// Get normalized ADC value (white base / black line / resolution=100)
void ADC_normalize(void) {
	
	double temp_1=0, temp_2=0;

	for(i=IR_NUMBER_m ; i<IR_NUMBER_M ; i++) {

		temp_1 = ADC_MAX[i] - ADC_DATA[i];
		temp_2 = ADC_MAX[i] - ADC_MIN[i];
		NORM_DATA[i] = (temp_1/temp_2) * RESOLUTION;

	}

}

void ADC_weight(void) {

	int weight[8] = {8, 4, 2, 1, -1, -2, -4, -8};
	
	for(i=IR_NUMBER_m ; i<IR_NUMBER_M ; i++) {
		WEIGHT_DATA[i] = NORM_DATA[i] * weight[i];
	}	

}

void Encoder_get(void) {

	

}

void ADC_uart(void) {
	
	Uart_Putch0('D');
	Uart_Putch0('a');
	Uart_Putch0('t');
	Uart_Putch0('a');

	for(i=IR_NUMBER_m ; i<IR_NUMBER_M ; i++) {
		j++;						

		tho = ADC_DATA[i]/1000 + 0x30;
		hnd = ADC_DATA[i]/100;
		hnd = hnd%10 + 0x30;
		ten = ADC_DATA[i]/10;
		ten = ten%10 + 0x30;
		one = ADC_DATA[i]%10 + 0x30;

		//---ADC_DATA[i]---//
		
		//Uart_Putch0('[');
		//Uart_Putch0(j+0x30);
		//Uart_Putch0(']');
		//Uart_Putch0('=');
		Uart_Putch0(tho);
		Uart_Putch0(hnd);
		Uart_Putch0(ten);
		Uart_Putch0(one);
		Uart_Putch0(' ');
		Uart_Putch0(' ');
		Uart_Putch0(0x09);
		
		/*//---ADC_MAX[i]---//
		tho = ADC_MAX[i]/1000 + 0x30;
		hnd = ADC_MAX[i]/100;
		hnd = hnd%10 + 0x30;
		ten = ADC_MAX[i]/10;
		ten = ten%10 + 0x30;
		one = ADC_MAX[i]%10 + 0x30;

		Uart_Putch0('M');
		Uart_Putch0('a');
		Uart_Putch0('x');
		Uart_Putch0('[');
		Uart_Putch0(j+0x30);
		Uart_Putch0(']');
		Uart_Putch0('=');
		Uart_Putch0(tho);
		Uart_Putch0(hnd);
		Uart_Putch0(ten);
		Uart_Putch0(one);
		Uart_Putch0(' ');
		Uart_Putch0(' ');

		//---ADC_MIN---//
		tho = ADC_MIN[i]/1000 + 0x30;
		hnd = ADC_MIN[i]/100;
		hnd = hnd%10 + 0x30;
		ten = ADC_MIN[i]/10;
		ten = ten%10 + 0x30;
		one = ADC_MIN[i]%10 + 0x30;

		Uart_Putch0('M');
		Uart_Putch0('i');
		Uart_Putch0('n');
		Uart_Putch0('[');
		Uart_Putch0(j+0x30);
		Uart_Putch0(']');
		Uart_Putch0('=');
		Uart_Putch0(tho);
		Uart_Putch0(hnd);
		Uart_Putch0(ten);
		Uart_Putch0(one);
		Uart_Putch0(' ');
		Uart_Putch0(' ');*/	

		//---NORM_DATA[i]---//
		/*hnd = NORM_DATA[i]/100;
		hnd = hnd%10 + 0x30;
		ten = NORM_DATA[i]/10;
		ten = ten%10 + 0x30;
		one = NORM_DATA[i]%10 + 0x30;

		Uart_Putch0('N');
		Uart_Putch0('o');
		Uart_Putch0('m');
		Uart_Putch0('[');
		Uart_Putch0(j+0x30);
		Uart_Putch0(']');
		Uart_Putch0('=');
		Uart_Putch0(hnd);
		Uart_Putch0(ten);
		Uart_Putch0(one);
		Uart_Putch0(' ');
		Uart_Putch0(' ');*/

		//---WEIGHT_DATA[i]---///
		/*tho = WEIGHT_DATA[i]/1000 + 0x30;
		hnd = WEIGHT_DATA[i]/100;
		hnd = hnd%10 + 0x30;
		ten = WEIGHT_DATA[i]/10;
		ten = ten%10 + 0x30;
		one = (int)WEIGHT_DATA;
		one = one%10 + 0x30;

		Uart_Putch0('W');
		Uart_Putch0('e');
		Uart_Putch0('i');
		Uart_Putch0('[');
		Uart_Putch0(j+0x30);
		Uart_Putch0(']');
		Uart_Putch0('=');
		Uart_Putch0(tho);
		Uart_Putch0(hnd);
		Uart_Putch0(ten);
		Uart_Putch0(one);
		Uart_Putch0(' ');
		Uart_Putch0(' ');*/
	}

	Uart_Putch0(0x0A);
	Uart_Putch0(0x0D);
	
	j=0;

}
