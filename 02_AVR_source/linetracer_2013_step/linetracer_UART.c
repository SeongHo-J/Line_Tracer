#include"linetracer_UART.h"

/* function of transmiting the data to computer */
void Uart_Putch0( unsigned char data )
{
	
	while ( !( UCSR0A & (1<<UDRE0)) ); /* Wait for empty transmit buffer */
	
	UDR0 = data; /* Put data into buffer, sends the data */
}

/* function of transmiting the data to MCU */
unsigned char Uart_Getch0( void )
{
	
	while ( !(UCSR0A & (1<<RXC0))); /* Wait for data to be received */
	
	return UDR0; /* Get and return received data from buffer */
}

void Uart_U16Bit_PutNum0( unsigned int NumData ) 
{ 
 unsigned int TempData; 
     //TempData = (NumData%100000)/10000; 
     //Uart_Putch0(TempData+48);                 // 10000

     //TempData = (NumData%10000)/1000; 
     //Uart_Putch0(TempData+48);                 // 1000
  
     TempData = (NumData%1000)/100; 
     Uart_Putch0(TempData+48);                 // 100
  
     TempData = (NumData%100)/10; 
     Uart_Putch0(TempData+48);                  // 10
     TempData = NumData%10; 
  Uart_Putch0(TempData+48);

  /* minus number output fuction */
 	/*if('VARIABLE' >= 0)
	{
		Uart_U16Bit_PutNum0('VARIABLE');
	}
	else
	{
		'VARIABLE' = -1* 'VARIABLE';
		Uart_Putch0('-');
		Uart_U16Bit_PutNum0('VARIABLE');
		'VARIABLE' = -1*'VARIABLE';
	}*/

}
