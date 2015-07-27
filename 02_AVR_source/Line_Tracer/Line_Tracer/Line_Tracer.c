#define F_CPU 16000000

#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/io.h>

int main(void)
{
	init_PWM();
	init_MCU();
	
	
	while(1)
	{
		MOTER_test();			
		
	}
	
}



