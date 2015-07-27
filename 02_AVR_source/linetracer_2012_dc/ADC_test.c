#include<avr/io.h>
#include<util/delay.h>

void init_LCD(void);
void LCD_wr_cmd(char cmd);
void LCD_wr_data(char data);
void LCD_wr_string(char d_line, char *lcd_str);

int main(void) {

	DDRA = 0xFF;
	DDRF = 0xFF;

	PORTA = 0x00;
	PORTF = 0x00;

	init_LCD();

	

	char* string = "haeri";

	while(1) {
		
		LCD_wr_string(0x80, string);

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
