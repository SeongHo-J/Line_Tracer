#define F_CPU 16000000

#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/io.h>

void init_PWM(void);
void init_MCU(void);
void MOTER_test(void);

void init_MCU()
{
	DDRC = 0xff;
	PORTC = 0x00;
	
}

void init_PWM(void)
{
	DDRB = 0xFF;
	TCCR0 = 0x6F;
	TCCR2 = 0x6D;
	TCNT0 = 0xff;
	TCNT2 = 0xff;
	OCR0 = 0xff;
	OCR2 = 0xff;
	
}

void MOTER_test(void){
	
	OCR0 = 0xff;
	OCR2 = 0xff;
	_delay_ms(1000);
	
	OCR0 = 0x80;
	OCR2 = 0x80;
	_delay_ms(1000);
	
	PORTC = 0x44;
	_delay_ms(1000);
	PORTC = 0x00;	
	}