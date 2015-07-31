
void MOTOR_speed(int lm_speed, int rm_speed) {	
	
	// because the maximum value of OCR1A and OCR1B is 16000
	if(lm_speed > 15500) lm_speed = 16000;
	
	else if(lm_speed < -15500) lm_speed = -16000;

	if(rm_speed > 15500) rm_speed = 16000;
	
	else if(rm_speed < -15500) rm_speed = -16000;


	if(lm_speed >= 0) {
		sbi(PORTC, PORTC4); // nomal direction
		OCR1A = lm_speed;
	}
	else if(lm_speed < 0) {
		cbi(PORTC, PORTC4); // reverse direction
		OCR1A = -1 * lm_speed;
	}
	
	if(rm_speed >= 0) {
		cbi(PORTA, PORTA2); // nomal direction
		OCR1B = rm_speed;
	}
	else if(rm_speed < 0 ) {
		sbi(PORTA, PORTA2); // reverse direction
		OCR1B = -1 * rm_speed;
	}

}

void MOTOR_control(void) {
	
	if(SENSOR_DATA > 0 ) {
		MOTOR_speed(Cruze_speed + SENSOR_DATA, Cruze_speed - SENSOR_DATA);
	}
	else if(SENSOR_DATA < 0) {
		MOTOR_speed(Cruze_speed - SENSOR_DATA, Cruze_speed + SENSOR_DATA);
	}

}
