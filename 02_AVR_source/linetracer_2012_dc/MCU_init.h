/* initialize function of linetracer*/

/* Bit set or clear */
#define sbi(PORTX,BitX) PORTX|=(1<<BitX)	// Bit set function
#define cbi(PORTX,BitX)	PORTX&=~(1<<BitX)	// Bit clear fuction

/* PORTA initialize*/
void init_PORT(void) {
	DDRA = 0xFF; // IR_sense emitter
	DDRB = 0xFF; // PWM_left = PB5(OC1A) PWM_right = PB6(OC1B)
	DDRC = 0x07; // LED0=PC0, LED1=PC1, DIR_left=PC2=1, DIR_right=PC3=0
	DDRD = 0x00; // SW(INT0)=PD0
	DDRE = 0x0E; // USART RxD0=PE0, TxD0=PE1
				 // encoder_l_A(INT4)=PE4 encoder_l_B(INT5)=PE5, encoder_r_A(INT6)=PE6, encoder_r_B(INT7)=PE7
	DDRF = 0x00; // ADCn
	
	PORTA = 0x00; // IR_sense emitter off
	PORTC = 0x00; //  LED0,1 off, DIR_r,l = 0 
}

/* external interrupt initialize */
void init_EX_INTERRUPT(void) {
	
	EICRA	= (0<<ISC31) |(0<<ISC30) |(0<<ISC21) |(0<<ISC20) |(0<<ISC11) |(0<<ISC10) |(1<<ISC01) |(0<<ISC00);
	EICRB	= (0<<ISC71) |(0<<ISC70) |(0<<ISC61) |(0<<ISC60) |(0<<ISC51) |(0<<ISC50) |(0<<ISC41) |(0<<ISC40);
	EIMSK	= (0<<INT7 ) |(0<<INT6 ) |(0<<INT5 ) |(0<<INT4 ) |(0<<INT3 ) |(0<<INT2 ) |(0<<INT1 ) |(1<<INT0 );
}

/* timer counter initialize */
void init_TIMER(void) {

	//TCCR3A	= (0<<COM3A1)|(0<<COM3A0)|(0<<COM3B1)|(0<<COM3B0)|(0<<COM3C1)|(0<<COM3C0)|(1<<WGM31) |(0<<WGM30);
	//TCCR3B	= (0<<ICNC1) |(0<<ICES1) |			  (1<<WGM13) |(1<<WGM12) |(0<<CS12)  |(1<<CS11)  |(1<<CS10) ;
	//ETIMSK	= 			 |			 |(0<<TICIE3)|(0<<OCIE3A)|(0<<OCIE3B)|(1<<TOIE3) |(0<<OCIE3C)|(0<<OCIE1C);

	TCCR3A = 0x02; // COMnx1:0 nomal mode , WGM3 1:0 fast PWM with ICRn TOP
	TCCR3B = 0x1A; // WGM3 3:2 fast PWM with ICRn TOP, 8 prescaler
	ETIMSK = 0x04; // Timer/Counter3 overflow interrupt enable(TOIE3)

	ICR3 = 40000; // ICRH and ICRL / 16Mhz crystal, 20ms control period, 8 prescaler
}

/* PWM initialize */
void init_PWM(void) {

	TCCR1A	= (1<<COM1A1)|(0<<COM1A0)|(1<<COM1B1)|(0<<COM1B0)|(0<<COM1C1)|(0<<COM1C0)|(1<<WGM11) |(0<<WGM10);
	TCCR1B	= (0<<ICNC1) |(0<<ICES1) |			  (1<<WGM13) |(1<<WGM12) |(0<<CS12)  |(1<<CS11)  |(0<<CS10) ;
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
	//ADCSRA = (1<<ADEN) |(0<<ADSC) |(0<<ADFR) |(0<<ADFR)|(0<<ADIE)|(0<<ADPS2)|(0<<ADPS1)|(0<<ADPS0);

	ADMUX = 0x00; // internal 2.56 voltage reference
	ADCSRA = 0x87; // prescaler : 2 division factor

}

/* function of transmiting the data to computer */
void Uart_Putch0( unsigned char data )
{
	
	while ( !( UCSR0A & (1<<UDRE0)) ); /* Wait for empty transmit buffer */
	
	UDR0 = data; /* Put data into buffer, sends the data */
}

/* function of transmiting the data to MCU */
unsigned char Uart_Getch0( void )
{
	
	while ( !(UCSR0A & (1<<RXC0))); /* Wait for data to be received */
	
	return UDR0; /* Get and return received data from buffer */
}

void Uart_U16Bit_PutNum0( unsigned int NumData ) 
{ 
 unsigned int TempData; 
     TempData = (NumData%100000)/10000; 
     Uart_Putch0(TempData+48);                 // 10000

     TempData = (NumData%10000)/1000; 
     Uart_Putch0(TempData+48);                 // 1000
  
     TempData = (NumData%1000)/100; 
     Uart_Putch0(TempData+48);                 // 100
  
     TempData = (NumData%100)/10; 
     Uart_Putch0(TempData+48);                  // 10
     TempData = NumData%10; 
  Uart_Putch0(TempData+48);

  /* minus number output fuction */
 	/*if('VARIABLE' >= 0)
	{
		Uart_U16Bit_PutNum0('VARIABLE');
	}
	else
	{
		'VARIABLE' = -1* 'VARIABLE';
		Uart_Putch0('-');
		Uart_U16Bit_PutNum0('VARIABLE');
		'VARIABLE' = -1*'VARIABLE';
	}*/

}


