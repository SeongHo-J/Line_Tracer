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
void ADC_uart(void);

// State / 20ms control period
SIGNAL(SIG_OVERFLOW3) {
	
	cli();
			
		ADC_get();
		ADC_uart();

	sei();
}

int main(void) {
	
	init_PORT();
	init_TIMER();
	init_EX_INTERRUPT();
	init_ADC();
	init_USART();

	sei();

	while(1) {
	
	}	


	return 0;
}

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

void ADC_uart(void) {
	
	Uart_Putch0('D');
	Uart_Putch0('a');
	Uart_Putch0('t');
	Uart_Putch0('a');
	Uart_Putch0(0x09);

	for(i=IR_NUMBER_m ; i<IR_NUMBER_M ; i++) {
		j++;						

		tho = ADC_DATA[i]/1000 + 0x30;
		hnd = ADC_DATA[i]/100;
		hnd = hnd%10 + 0x30;
		ten = ADC_DATA[i]/10;
		ten = ten%10 + 0x30;
		one = ADC_DATA[i]%10 + 0x30;

		//---ADC_DATA[i]---//

		Uart_Putch0(tho);
		Uart_Putch0(hnd);
		Uart_Putch0(ten);
		Uart_Putch0(one);
		Uart_Putch0(' ');
		Uart_Putch0(' ');
		Uart_Putch0(0x09);
	}

	Uart_Putch0(0x0A);
	Uart_Putch0(0x0D);
	
	j=0;

}
