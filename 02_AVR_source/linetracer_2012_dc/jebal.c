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
#define DELAY 1.5

// State and Usart variable
volatile int state = 0, i=0, j=0;

// ADC data variable
volatile int ADC_DATA[8] = {0,};

void ADC_get(void);
void ADC_uart(void);

int main(void) {
	
	init_PORT();
	init_EX_INTERRUPT();
	init_TIMER();
	init_ADC();
	init_USART();

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

			case IDLE :
				
				PORTC = 0x00; // IDLE state check

			break;
			

			case NORM :
				
				PORTC = 0x01; // NORM state check

				ADC_get();

				ADC_uart();

			break;



			case WAIT :
				
				PORTC = 0x02; // WAIT check

			break;



			case RUN :
				
				PORTC = 0x03; // RUN check
				
			break;
		
	}

	sei();
}

// Get ADC from IR sensor
void ADC_get(void) {


	PORTA = 0x01;

	_delay_us(DELAY);

	ADCSRA |= 0xC0; // ADEN=1, ADSC=1
	while( ((ADCSRA&0x10) == 0x00) ); // wait for ADC complete
	ADC_DATA[0] = ADC;
		
	PORTA = 0x00;
				
	ADMUX = 0xC0; // prepare for ADC_4
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

}
