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
	LCD_wr_cmd(0x38);//��ɼ�(�����͹��� 8��Ʈ, ���μ�:2��)
	_delay_ms(20);
	LCD_wr_cmd(0x38);//��Ȯ�� ������ ���� ������ ���� �¾� ���� ��ִ´�.
	_delay_ms(20);
	LCD_wr_cmd(0x38);
	LCD_wr_cmd(0x04); //��Ʈ������
	LCD_wr_cmd(0x0C); //ǥ�� On
	LCD_wr_cmd(0x01); //ȭ�� �����
}

void LCD_wr_cmd(char cmd) {
	PORTG = 0x04; // ��ɾ�� E=1(enable on ), RW=0(write), RS=0(command register)
	PORTA = cmd; //PORTB�� �����͹����� ����Ǿ� �ִ�.
	PORTG = PORTG^0x04; //E ��ȣ�� H->L�� �ϱ� ���� : LCD ���ο� ��� ���� off
	_delay_ms(2); //LCD ���ε��� �ð�
}

void LCD_wr_data(char data) { 
	PORTG = 0x05; //�����;��� E=1(enable on ), RW=0(write), RS=1(data register) 
	PORTA = data; //PORTB�� �����͹����� ����Ǿ� �ִ�. 
	PORTG = PORTG^0x04; //E ��ȣ�� H->L�� �ϱ� ���� 
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
