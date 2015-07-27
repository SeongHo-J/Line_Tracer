#include<avr/io.h>

void init_PWM(void);

int main(void) {

	init_PWM();
       
	DDRC = 0xC0;
	DDRD = 0xFF;

	PORTC = 0x3F;
	PORTD = 0x00;

	while(1) {

		if(PINC == 0x3E) {
			PORTD = 0x04;
			OCR0 = 0x00;
		}
		else if(PINC == 0x3D) {
			PORTD = 0x08;
			OCR0 = 0x40;

		}
		else if(PINC == 0x3B) {
			PORTD = 0x10;
			OCR0 = 0xFF;
		}

	}

	return 0;

}

void init_PWM(void) {
	
	DDRB = 0xF0;
	
	TCCR0 = 0x6F;
	TCNT0 = 0xFF;
	OCR0 = 0x00;

}
