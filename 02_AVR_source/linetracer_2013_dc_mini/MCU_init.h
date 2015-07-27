#ifndef _MCU_INIT_H_
#define _MCU_INIT_H_
/* initialize function of linetracer*/

/* Bit set or clear */
#define sbi(PORTX,BitX) PORTX|=(1<<BitX)	// Bit set function
#define cbi(PORTX,BitX)	PORTX&=~(1<<BitX)	// Bit clear fuction

/* PORT initialize*/
void init_PORT(void) {

	DDRB = 0x0E; // [2] motor_left [1] motor_right
	DDRC = 0x00; // [6] reset switch [3:0]IR receiver 
	DDRD = 0xF3; // [7:4] IR emitter [1:0] LED 

	PORTD = 0x00;
	PORTB = 0x00;
}

/* external interrupt initialize */

void init_EX_INTERRUPT(void) {
	
	MCUCR	= (0<<ISC11) |(0<<ISC10) |(1<<ISC01) |(0<<ISC00);
	GICR	= (0<<INT1 ) |(1<<INT0 );
}


/* timer counter initialize */
void init_TIMER2(void) {
	
	TCCR2 = (0<<FOC2)  |(0<<WGM20) |(0<<COM21) |(0<<COM20) |(1<<WGM21) |(1<<CS22)  |(0<<CS21)  |(1<<CS20);
	TIMSK = (1<<OCIE2) |(0<<TOIE2) |(0<<TICIE1)|(0<<OCIE1A)|(0<<OCIE1B)|(0<<TOIE1) |(0<<TOIE0);
	TCNT2 = 0x00;
	OCR2  = 150;


}

/*
void init_TIMER3(void) {

	//TCCR3A	= (0<<COM3A1)|(0<<COM3A0)|(0<<COM3B1)|(0<<COM3B0)|(0<<COM3C1)|(0<<COM3C0)|(1<<WGM31) |(0<<WGM30);
	//TCCR3B	= (0<<ICNC1) |(0<<ICES1) |			  (1<<WGM13) |(1<<WGM12) |(0<<CS12)  |(1<<CS11)  |(1<<CS10) ;
	//ETIMSK	= 			 |			 |(0<<TICIE3)|(0<<OCIE3A)|(0<<OCIE3B)|(1<<TOIE3) |(0<<OCIE3C)|(0<<OCIE1C);

	TCCR3A = 0x02; // COMnx1:0 nomal mode , WGM3 1:0 fast PWM with ICRn TOP
	TCCR3B = 0x1B; // WGM3 3:2 fast PWM with ICRn TOP, 64 prescaler
	ETIMSK = 0x04; // Timer/Counter3 overflow interrupt enable(TOIE3)

	//ICR3 = 12500; // ICRH and ICRL / 16Mhz crystal, 50ms control period, 64 prescaler
	ICR3 = 5000; // ICRH and ICRL / 16Mhz crystal, 10ms control period, 64 prescaler
}
*/

/* PWM initialize */
void init_PWM(void) {

	TCCR1A	= (0<<COM1A1)|(0<<COM1A0)|(0<<COM1B1)|(0<<COM1B0)|(0<<FOC1A) |(0<<FOC1B) |(0<<WGM11) |(1<<WGM10);
	TCCR1B	= (0<<ICNC1) |(0<<ICES1) |			  (0<<WGM13) |(1<<WGM12) |(0<<CS12)  |(1<<CS11)  |(0<<CS10) ;
	TIMSK	= (1<<OCIE2) |(0<<TOIE2) |(0<<TICIE1)|(0<<OCIE1A)|(0<<OCIE1B)|(0<<TOIE1) |(0<<TOIE0);
	TCNT1	= 0x00;
	
	//ICR1= 2000; // 1khz

	OCR1A = 0x0000;
	OCR1B = 0x0000;

}

/* USART initialize */
/*
void init_USART(void) {

	//UCSR0B	= (0<<RXCIE0)|(0<<TXCIE0)|(0<<UDRIE0)|(1<<RXEN0) |(1<<TXEN0) |(0<<UCSZ02)|(0<<RXB80) |(0<<TXB80);
	//UCSR0C	=             (0<<UMSEL0)|(0<<UPM01) |(0<<UPM00) |(0<<USBS1) |(1<<UCSZ01)|(1<<UCSZ00)|(0<<UCPOL0);
	//UBRR0H	= 0;
	//UBRR0L	= 8;
	
	UBRR0H = 0;		
	UBRR0L = 8; // f_osc = 16 Mhz , Baud rate = 115200(bps)
	UCSR0B = 0x18; // RXEN=1, TXEN=1
	UCSR0C = 0x06; // Asynchronous / No parity / 1 stop bit / 8 bit data size
}
*/

/* ADC initialize */
void init_ADC(void) {
	
	//ADMUX  = (1<<REFS1)|(1<<REFS0)|(0<<ADLAR)|(0<<MUX3)|(0<<MUX2) |(0<<MUX1) |(0<<MUX0);
	//ADCSRA = (1<<ADEN) |(0<<ADSC) |(0<<ADFR) |(0<<ADFR)|(1<<ADIE)|(0<<ADPS2)|(0<<ADPS1)|(0<<ADPS0);

	ADMUX = 0xC0; // internal 2.56 voltage reference
	ADCSRA = 0x87; // ADC enable(ADEN=1) / prescaler : 128 division factor

}

#else
#endif

