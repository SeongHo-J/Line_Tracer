#ifndef _MCU_INIT_H_
#define _MCU_INIT_H_
/* initialize function of linetracer*/

/* Bit set or clear */
#define sbi(PORTX,BitX) PORTX|=(1<<BitX)	// Bit set function
#define cbi(PORTX,BitX)	PORTX&=~(1<<BitX)	// Bit clear fuction

/* PORT initialize*/
void init_PORT(void) {

	DDRA = 0x0F; // LED[3:0]
	DDRC = 0xFF; // Motor_R[3:0]
	DDRD = 0xFF; // IR_sensor emitter
	DDRF = 0x00; // IR_sensor receiver
	DDRE = 0x02; // SW=INT4, RxD0=PE0, TxD0=PE1
	
	PORTA = 0x00; // LED off
	PORTD = 0x00; // IR_sensor emitter off
}

/* external interrupt initialize */
void init_EX_INTERRUPT(void) {
	
	EICRA	= (0<<ISC31) |(0<<ISC30) |(0<<ISC21) |(0<<ISC20) |(0<<ISC11) |(0<<ISC10) |(0<<ISC01) |(0<<ISC00);
	EICRB	= (0<<ISC71) |(0<<ISC70) |(0<<ISC61) |(0<<ISC60) |(0<<ISC51) |(0<<ISC50) |(1<<ISC41) |(0<<ISC40);
	EIMSK	= (0<<INT7 ) |(0<<INT6 ) |(0<<INT5 ) |(1<<INT4 ) |(0<<INT3 ) |(0<<INT2 ) |(0<<INT1 ) |(0<<INT0 );
}

/* timer counter initialize */
void init_TIMER0(void) {
	
	TCCR0 = (0<<FOC0)  |(0<<WGM00) |(0<<COM01) |(0<<COM00) |(1<<WGM01) |(1<<CS02)  |(0<<CS01)  |(1<<CS00);
	TIMSK = (0<<OCIE2) |(0<<TOIE2) |(0<<TICIE1)|(0<<OCIE1A)|(0<<OCIE1B)|(0<<TOIE1) |(0<<OCIE0) |(0<<TOIE0);
	TCNT0 = 0x00;
	OCR0  = 0x00;


}

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

/* PWM initialize */
void init_PWM(void) {

	TCCR1A	= (1<<COM1A1)|(0<<COM1A0)|(1<<COM1B1)|(0<<COM1B0)|(0<<COM1C1)|(0<<COM1C0)|(1<<WGM11) |(0<<WGM10);
	TCCR1B	= (0<<ICNC1) |(0<<ICES1) |			  (1<<WGM13) |(1<<WGM12) |(1<<CS12)  |(0<<CS11)  |(0<<CS10) ;
	TIMSK	= (0<<OCIE2) |(0<<TOIE2) |(0<<TICIE1)|(0<<OCIE1A)|(0<<OCIE1B)|(0<<TOIE1) |(0<<OCIE0) |(0<<TOIE0);
	

	ICR1= 2000; // 1khz

	OCR1A = 0x0000;
	OCR1B = 0x0000;

}

/* USART initialize */
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

/* ADC initialize */
void init_ADC(void) {
	
	//ADMUX  = (1<<REFS1)|(1<<REFS0)|(0<<ADLAR)|(0<<MUX4)|(0<<MUX3)|(0<<MUX2) |(0<<MUX1) |(0<<MUX0);
	//ADCSRA = (1<<ADEN) |(0<<ADSC) |(0<<ADFR) |(0<<ADFR)|(1<<ADIE)|(0<<ADPS2)|(0<<ADPS1)|(0<<ADPS0);

	ADMUX = 0xC0; // internal 2.56 voltage reference
	ADCSRA = 0x87; // ADC enable(ADEN=1) / prescaler : 128 division factor

}


#else
#endif

