#include"linetracer_MOTOR.h"
/*
void MOTOR(const int dir, const int pulse, const int speed) {
	
	// 1-2 phase excitation //
	// 2 clock(step) = 1.8 degree //
	//int step_f[8] = {0x19, 0x58, 0x4A, 0x62, 0x26, 0xA4, 0x85, 0x91};
	//int step_r[8] = {0x10, 0x50, 0x40, 0x60, 0x20, 0xA0, 0x80, 0x90};
	//int step_l[8] = {0x09, 0x08, 0x0A, 0x02, 0x06, 0x04, 0x05, 0x01};

	// 2 phase excitation //
	// 1 clock(step) = 1.8 degree //
	int step_f[4] = {0x59, 0x6A, 0xA6, 0x95};
	int step_r[4] = {0x50, 0x60, 0xA0, 0x90};
	int step_l[4] = {0x09, 0x0A, 0x06, 0x05};

	int n=0;
	int temp=0;

	switch(dir) {
		
		case FORWARD :
			
			while(n<pulse) {			
				PORTC = step_f[temp];
				temp = (++n)%PH_EX;

				_delay_ms(speed);
			}

			break;

		case BACKWARD :
			
			while(n<pulse) {
				PORTC = step_f[PH_EX-temp];
				temp = (++n)%PH_EX;

				_delay_ms(speed);
			}

		case RIGHT :

			while(n<pulse) {
				PORTC = step_l[temp]; // if you want to go right side, you should operate left side motor
				temp = (++n)%PH_EX;

				_delay_ms(speed);

			}

		case LEFT :

			while(n<pulse) {
			
				PORTC = step_r[temp]; // if you want to go left side, you should operate right side motor
				temp = (++n)%PH_EX;
				
				_delay_ms(speed);

			}


	}

}
*/

void MOTOR(char *step, const char dir, const int speed) {
	
	// 1-2 phase excitation //
	// 2 clock(step) = 1.8 degree //
	//int step_f[8] = {0x19, 0x58, 0x4A, 0x62, 0x26, 0xA4, 0x85, 0x91};
	//int step_r[8] = {0x10, 0x50, 0x40, 0x60, 0x20, 0xA0, 0x80, 0x90};
	//int step_l[8] = {0x09, 0x08, 0x0A, 0x02, 0x06, 0x04, 0x05, 0x01};

	// 2 phase excitation //
	// 1 clock(step) = 1.8 degree //
	//int step_f[4] = {0x59, 0x6A, 0xA6, 0x95};
	//int step_r[4] = {0x50, 0x60, 0xA0, 0x90};
	//int step_l[4] = {0x09, 0x0A, 0x06, 0x05};

	OCR0 = speed;

	switch(dir) {
		
		case FORWARD :

			switch(*step) {
				case 0x59 : *step = 0x6A; break;
				case 0x6A : *step = 0xA6; break;
				case 0xA6 : *step = 0x95; break;
				case 0x95 : *step = 0x59; break;
				default   : *step = 0x59;
			}

			break;

		case RIGHT :

			switch(*step) {
				case 0x09 : *step = 0x0A; break;
				case 0x0A : *step = 0x06; break;
				case 0x06 : *step = 0x05; break;
				case 0x05 : *step = 0x09; break;
				default   : *step = 0x09;
			}

			break;

		case LEFT :

			switch(*step) {
				case 0x50 : *step = 0x60; break;
				case 0x60 : *step = 0xA0; break;
				case 0xA0 : *step = 0x90; break;
				case 0x90 : *step = 0x50; break;
				default   : *step = 0x50;
			}

			break;


	}

	PORTC = *step;

}


void MOTOR_R(const int dir, const int rad) {
	
	// 1-2 phase excitation //
	// 2 clock(step) = 1.8 degree //
	int step[8] = {0x10, 0x50, 0x40, 0x60, 0x20, 0xA0, 0x80, 0x90};

	// 2 phase excitation //
	// 1 clock(step) = 1.8 degree //
	//int step_r[4] = {0x50, 0x60, 0xA0, 0x90};

	int n=0;
	int temp=0;
	
	// Forward
	if(dir==0) {
		
		while(n<rad) {
			
			PORTC = step[temp];
			temp = (++n)%PH_EX;

			_delay_ms(RS);

		}

	}
	// Backward
	else if(dir==1) {
		
		while(n<rad) {
			
			PORTC = step[7-temp];
			temp = (++n)%PH_EX;

			_delay_ms(RS);
		}


	}

}

void MOTOR_L(const int dir, const int rad) {
	
	// 1-2 phase excitation //
	// 2 clock(step) = 1.8 degree //
	int step[8] = {0x09, 0x08, 0x0A, 0x02, 0x06, 0x04, 0x05, 0x01};

	// 2 phase excitation //
	// 1 clock(step) = 1.8 degree //
	//int step_l[4] = {0x09, 0x0A, 0x06, 0x05};

	int n=0;
	int temp=0;
	
	// Forward
	if(dir==0) {
		
		while(n<rad) {
			
			PORTC = step[temp];
			temp = (++n)%8;

			_delay_ms(RS);

		}

	}
	// Backward
	else if(dir==1){
		
		while(n<rad) {
			
			PORTC = step[7-temp];
			temp = (++n)%8;

			_delay_ms(RS);
		}


	}

}
