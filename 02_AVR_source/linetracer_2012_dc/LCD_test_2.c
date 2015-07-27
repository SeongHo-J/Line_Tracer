#include<avr/io.h>
#include<util/delay.h>
#include<avr/interrupt.h>

void init_LCD(void);
void init_ADC(void);
void init_timer(void);

void LCD_wr_cmd(char cmd);
void LCD_wr_data(char data);
void LCD_wr_string(char d_line, char *lcd_str);

int ir_sense(void);

int count=0;

int ir = 0;

int main(void) {
	
	DDRA = 0xFF; // CLCD
	DDRD = 0xFF; // LED
	DDRG = 0xFF; // CLCD LCD_EN, LCD_RW, LCD_RS
	
	PORTD = 0x00;

	init_LCD();
	init_ADC();
	init_timer();

	char *string1 = "ADC=";
	//int ir[10] = {0,};
	int ten=0, one=0;

	sei();



	while(1) {
		

			//ir[i] = ir_sense();

			ten = ir/10 + 0x30;
			one = ir%10 + 0x30;

			LCD_wr_string(0x80, string1);
			LCD_wr_data(ten);
			LCD_wr_data(one);

			_delay_ms(10);


	}


	return 0;

}

void init_LCD(void) {
	_delay_ms(20);
	LCD_wr_cmd(0x38);//기능셋(데이터버스 8비트, 라인수:2줄)
	_delay_ms(20);
	LCD_wr_cmd(0x38);//정확한 동작을 위해 여러번 같은 셋업 값을 써넣는다.
	_delay_ms(20);
	LCD_wr_cmd(0x38);
	LCD_wr_cmd(0x04); //엔트리모드셋
	LCD_wr_cmd(0x0C); //표시 On
	LCD_wr_cmd(0x01); //화면 지우기
}

void LCD_wr_cmd(char cmd) {
	PORTG = 0x04; // 명령어쓰기 E=1(enable on ), RW=0(write), RS=0(command register)
	PORTA = cmd; //PORTB에 데이터버스가 연결되어 있다.
	PORTG = PORTG^0x04; //E 신호를 H->L로 하기 위해 : LCD 새로운 명령 제어 off
	_delay_ms(2); //LCD 내부동작 시간
}

void LCD_wr_data(char data) { 
	PORTG = 0x05; //데이터쓰기 E=1(enable on ), RW=0(write), RS=1(data register) 
	PORTA = data; //PORTB에 데이터버스가 연결되어 있다. 
	PORTG = PORTG^0x04; //E 신호를 H->L로 하기 위해 
	_delay_ms(2); 
}

void LCD_wr_string(char d_line, char *lcd_str) {
	LCD_wr_cmd(d_line);
	
	while(*lcd_str != '\0') {
		LCD_wr_data(*lcd_str);
		lcd_str++;
	}
}

void init_ADC(void) {
	DDRF = 0x00;
	
	ADMUX = 0xC1;
	ADCSRA = 0xA7;
 }

/*int ir_sense(void) {
		int cnt=0, tot=0, t=0, ir=0;

		for(cnt=0 ; cnt<32 ; cnt++) {
			ADCSRA |= 0xC0; // 0b=1100 0000 ADEN=1, ADSC=1
			while((ADCSRA&0x10) == 0x00); // ADIF=1
			tot += ADCL+ADCH*256;
			_delay_ms(1);
		}

		tot = (tot>>5);
		t = (tot/2.5);
		ir = t/10;

		return ir;
}*/

int ir_sense(void) {
		int tot=0, t=0, ir=0;

		ADCSRA |= 0xC0; // 0b=1100 0000 ADEN=1, ADSC=1
		while((ADCSRA&0x10) == 0x00); // ADIF=1
		tot = ADCL+ADCH*256;
		
		t = (tot/2.5);
		ir = t/10;

		return ir;
}

void init_timer(void) {
	TCCR2 = 0x05;
	TCNT2 = 0x00;
	TIMSK = 0x40;
}

ISR(TIMER2_OVF_vect) {

	count++;

	if(count == 4) {
		
		PORTD = 0x04;
		_delay_us(10);

		cli();
		
		ir = ir_sense(); 
		PORTD = 0x00;

		count = 0;

		sei();
	}
}
