
#include "UART0.h"
char receive_string[50];

void delay_ms(unsigned int j) /* Function for delay in milliseconds  */
{
    unsigned int x,i;
	for(i=0;i<j;i++)
	{
    for(x=0; x<6000; x++);    /* loop to generate 1 millisecond delay with Cclk = 60MHz */
	}
}

void UART0_init(void)
{
	PINSEL0 = PINSEL0 | 0x00000005;	/* Enable UART0 Rx0 and Tx0 pins of UART0 */
	U0LCR = 0x83;	/* DLAB = 1, 1 stop bit, 8-bit character length */
	U0DLM = 0x00;	/* For baud rate of 9600 with Pclk = 15MHz */
	U0DLL = 0x61;	/* We get these values of U0DLL and U0DLM from formula */
	U0LCR = 0x03; /* DLAB = 0 */
	delay_ms(5);
}

void UART0_TxChar(char ch) /* A function to send a byte on UART0 */
{
	U0THR = ch;
	while( (U0LSR & 0x40) == 0 );	/* Wait till THRE bit becomes 1 which tells that transmission is completed */
}

void UART0_SendString(char* str) /* A function to send string on UART0 */
{
	unsigned int i = 0;
	while( str[i] != '\0' )
	{
		UART0_TxChar(str[i]);
		i++;
	}
}

unsigned char UART0_RxChar(void) /* A function to receive a byte on UART0 */
{
	while( (U0LSR & 0x01) == 0);	/* Wait till RDR bit becomes 1 which tells that receiver contains valid data */
		return U0RBR;
}

void UART0_RxString(void) /* A function to receive string on UART0 */
{
	unsigned int i = 0;
	do{
	while( (U0LSR & 0x01) == 0);
			receive_string[i] = U0RBR;
		i++;
	}while( (U0RBR != 0x0D) );
}
