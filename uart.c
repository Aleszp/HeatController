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

void USART_Append_To_Buffer_int8(int8_t src, uint8_t wait, uint8_t space)
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
	TXBuf[0]=space?',':'\n';
	
	txindex=+5;
	UCSRB |= (1<<UDRIE);  	//odblokuj wysyłanie
	if(wait)
	{
		while(txindex>0);
	}
}

void USART_Append_To_Buffer_int16(int16_t src, uint8_t wait, uint8_t space)
{
	UCSRB &= ~(1<<UDRIE); 	//zablokuj wysyłanie na czas tej operacji
	int16_t tmp=src;
	if(src<0)
	{
		src*=-1;
		TXBuf[6]='-';
	}
	else
	{
		TXBuf[6]='+';
	}
	int16_t div=10000;
	for(int8_t ii=5;ii>0;ii--)
	{
		tmp=src/div;
		TXBuf[ii]='0'+tmp;	
		src-=div*tmp;
		div/=10;
	}
	TXBuf[0]=space?',':'\n';
	
	txindex=+7;
	UCSRB |= (1<<UDRIE);  	//odblokuj wysyłanie
	if(wait)
	{
		while(txindex>0);
	}
}

void USART_Append_To_Buffer_uint8(uint8_t src, uint8_t wait, uint8_t space)
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
	TXBuf[0]=space?',':'\n';
	txindex=+4;
	UCSRB |= (1<<UDRIE);  	//odblokuj wysyłanie
	if(wait)
	{
		while(txindex>0);
	}
}

void USART_Append_To_Buffer_uint32(int32_t src, uint8_t wait, uint8_t space)
{
	UCSRB &= ~(1<<UDRIE); 	//zablokuj wysyłanie na czas tej operacji
	int32_t tmp=src;
	int32_t div=1000000000;
	for(uint8_t ii=10;ii>0;ii--)
	{
		tmp=src/div;
		TXBuf[ii]='0'+tmp;	
		src-=div*tmp;
		div/=10;
	}
	TXBuf[0]=space?',':'\n';
	txindex=+11;
	UCSRB |= (1<<UDRIE);  	//odblokuj wysyłanie
	if(wait)
	{
		while(txindex>0);
	}
}
