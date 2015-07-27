#include<avr/io.h>
#include<util/delay.h>

void init_LCD(void);

void LCD_wr_cmd(char cmd);
void LCD_wr_data(char data);
void LCD_wr_string(char d_line, char *lcd_str);

int temp_sense(void);

int main(void) {
	
	DDRA = 0xFF;
	DDRB = 0xF0;
	DDRD = 0xFF;
	DDRG = 0xFF;

	PORTD = 0x00;

	while(1) {

		init_LCD();

		PORTD = 0x00;
		
		//LCD_wr_cmd(0x01);
		//LCD_wr_data(0x41);

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
