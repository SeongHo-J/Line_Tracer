#ifndef _LINETRACER_UART_H_
#define _LINETRACER_UART_H_

#include<avr/io.h>

void Uart_Putch0( unsigned char data );
unsigned char Uart_Getch0( void );
void Uart_U16Bit_PutNum0( unsigned int NumData );

#else

#endif
