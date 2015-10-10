#include "LineTracer_MOTER.h"
#include "linetracer_ADC.h"

void MOTER_test(const int *NORM_DATA){
	
	/*
	if(NORM_DATA[0]>=(int)(RESOLUTION*0.5)&&NORM_DATA[7]<=(int)(RESOLUTION*0.5)){
		PORTC = 0x41;		
	}
	else if(NORM_DATA[0]<=(int)(RESOLUTION*0.5)&&NORM_DATA[7]>=(int)(RESOLUTION*0.5)){
		PORTC = 0x05;		
	}
	else if(NORM_DATA[0]>=(int)(RESOLUTION*0.5)&&NORM_DATA[7]>=(int)(RESOLUTION*0.5)){
		PORTC = 0x45;		
	}
	else{
		PORTC = 0x01;	
	}*/
	PORTC = 0x01;
	
	
}

void MOTOR_speed(int speed, const double *SENSOR_DATA) {
	
	int temp = SENSOR_DATA[0]*5;
	if(temp > 0) {
		cbi(PORTC, PORTC6);
		cbi(PORTC, PORTC2);
		OCR0 = speed-temp;
		OCR2 = speed+temp;
	}
	else if(temp < 0) {
		cbi(PORTC, PORTC6);
		cbi(PORTC, PORTC2);
		OCR0 = speed-temp;
		OCR2 = speed+temp;
	}


	
}

void MOTOR_control(const double *SENSOR_DATA, const int Cruze_speed) {
		//int temp = SENSOR_DATA[0];
		MOTOR_speed(Cruze_speed,SENSOR_DATA);			
}  