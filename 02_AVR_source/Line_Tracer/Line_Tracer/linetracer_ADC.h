#ifndef _LINETRACER_ADC_H_
#define _LINETRACER_ADC_H_

#include<avr/io.h>
#include<util/delay.h>

#define IR_NUMBER_m 0
#define IR_NUMBER_M 7

#define RESOLUTION 100

void init_ADC(void);
void ADC_GET(int *ADC_DATA);
void ADC_MAX_MIN(int *ADC_MAX, int *ADC_MIN, const int *ADC_DATA);
void ADC_NORMALIZE(int *NORM_DATA, const int *ADC_DATA, const int *ADC_MAX, const int *ADC_MIN);
void DATA_OUT(const int *NORM_DATA);




#else

#endif
