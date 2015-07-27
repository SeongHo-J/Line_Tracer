#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>

volatile int i=0, j=0;

void init_port(void);
void init_interrupt(void);

int main(void) {
	
	init_port();
	init_interrupt();

	sei();

	while(1) {
		
		if( i+j==38 ) {
			
			PORTB = 0x80;
			_delay_ms(300);
			PORTB = 0x00;

			i=0;
			j=0;

		}
	
	}

	return 0;

}

ISR(INT4_vect) {
	i++;
	EIFR = 0x00;	
}

ISR(INT5_vect) {
	j++;
	EIFR = 0x00;
}

void init_port(void) {
	DDRB = 0xF0;
	DDRD = 0xFC;
	DDRE = 0x00;
	
	PORTB = 0x00;
	PORTE = 0xFF;
	PORTD = 0x00;
}

void init_interrupt(void) {
	EICRA = 0x00;
	EICRB = 0x0A;
	EIMSK = 0x30;
	EIFR = 0x00;
}
