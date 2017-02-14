#include <avr/io.h>
#include <avr/interrupt.h>

#include "global_variables.h"
#include "uart.h"

void USART_Init(uint16_t baud)
{
	UBRRH=(baud>>8);												//ustaw starszy bajt
	UBRRL=baud;														//ustaw młodszy bajt
	
	UCSRB = (1<<RXEN)|(1<<TXEN);									//odblokuj odbiór i nadawanie UART
	UCSRC = (1<<URSEL)|(1<<UCSZ1)|(1<<UCSZ0)|(1<<UPM1)|(1<<UPM0);	//8 bitów, 1 bit stopu, kontrola parzystości: nieparzysta
	
	UCSRB |= (1<<RXCIE);											//odblokuj przerwanie po odebraniu bajtu
}

void USART_Append_To_Buffer(char* src, uint8_t len, uint8_t wait)
{
	UCSRB &= ~(1<<UDRIE); 	//zablokuj wysyłanie na czas tej operacji
	
	uint8_t len_=len-1;
	
	for(uint8_t ii=0;ii<len;ii++)
	{	
		TXBuf[len_-ii]=src[ii];
	}
	txindex=+len;
	UCSRB |= (1<<UDRIE);  	//odblokuj wysyłanie
	if(wait)
	{
		while(txindex>0);
	}
}

void USART_Append_To_Buffer_int8(int8_t src, uint8_t wait)
{
	UCSRB &= ~(1<<UDRIE); 	//zablokuj wysyłanie na czas tej operacji
	int8_t tmp=src;
	int8_t sign=(src<0);
	if(sign)
		src*=-1;
	TXBuf[4]=sign?'-':'+';
	tmp=src/100;
	TXBuf[3]='0'+tmp;	
	src-=100*tmp;
	tmp=src/10;
	TXBuf[2]='0'+tmp;	
	src-=10*tmp;
	TXBuf[1]='0'+src;
	TXBuf[0]='\n';
	
	txindex=+5;
	UCSRB |= (1<<UDRIE);  	//odblokuj wysyłanie
	if(wait)
	{
		while(txindex>0);
	}
}

void USART_Append_To_Buffer_int16(int16_t src, uint8_t wait)
{
	UCSRB &= ~(1<<UDRIE); 	//zablokuj wysyłanie na czas tej operacji
	int16_t tmp=src;
	int8_t sign=(src<0);
	if(sign)
		src*=-1;
	TXBuf[7]=sign?'-':'+';
	tmp=src/10000;
	TXBuf[6]='0'+tmp;	
	src-=10000*tmp;
	tmp=src/1000;
	TXBuf[5]='0'+tmp;	
	src-=1000*tmp;
	TXBuf[4]='0'+src;
	tmp=src/100;
	TXBuf[3]='0'+tmp;	
	src-=100*tmp;
	tmp=src/10;
	TXBuf[2]='0'+tmp;	
	src-=10*tmp;
	TXBuf[1]='0'+src;
	TXBuf[0]='\n';
	
	txindex=+8;
	UCSRB |= (1<<UDRIE);  	//odblokuj wysyłanie
	if(wait)
	{
		while(txindex>0);
	}
}

void USART_Append_To_Buffer_uint8(uint8_t src, uint8_t wait)
{
	UCSRB &= ~(1<<UDRIE); 	//zablokuj wysyłanie na czas tej operacji
	uint8_t tmp=src;
	
	tmp=src/100;
	TXBuf[3]='0'+tmp;	
	src-=100*tmp;
	tmp=src/10;
	TXBuf[2]='0'+tmp;	
	src-=10*tmp;
	TXBuf[1]='0'+src;
	TXBuf[0]='\n';
	txindex=+4;
	UCSRB |= (1<<UDRIE);  	//odblokuj wysyłanie
	if(wait)
	{
		while(txindex>0);
	}
}
