#include "LineTracer_MOTER.h"
#include "linetracer_ADC.h"

void MOTER_test(const int *NORM_DATA){
	
	
	if(NORM_DATA[1]>=(int)(RESOLUTION*0.5)&&NORM_DATA[6]<=(int)(RESOLUTION*0.5)){
		PORTC = 0x40;		
	}
	else if(NORM_DATA[1]<=(int)(RESOLUTION*0.5)&&NORM_DATA[6]>=(int)(RESOLUTION*0.5)){
		PORTC = 0x04;		
	}
	else if(NORM_DATA[1]>=(int)(RESOLUTION*0.5)&&NORM_DATA[6]>=(int)(RESOLUTION*0.5)){
		PORTC = 0x44;		
	}
	else{
		PORTC = 0x00;	
	}
	
	
	
}

void MOTOR_Lspeed(int LM_speed, const double *SENSOR_DATA) {
	
	int temp = SENSOR_DATA[0];
	if(temp > 0) {
		sbi(PORTC, PORTC6);
		OCR2 = LM_speed;
	}
	else if(temp < 0) {
		cbi(PORTC, PORTC6);
		OCR2 = -1 * LM_speed;
	}


	
}


void MOTOR_Rspeed(int RM_speed, const double *SENSOR_DATA) {
	
	int temp = SENSOR_DATA[0];
	if(temp > 0) {
		cbi(PORTC, PORTC2);
		OCR0 = RM_speed;
	}
	else if(temp < 0 ) {
		sbi(PORTC, PORTC2);
		OCR0 = -1*RM_speed;
	}
}


void MOTOR_control(const double *SENSOR_DATA, const int Cruze_speed) {
		//int temp = SENSOR_DATA[0];
		MOTOR_Lspeed(Cruze_speed,SENSOR_DATA);
		MOTOR_Rspeed(Cruze_speed,SENSOR_DATA);		
}  