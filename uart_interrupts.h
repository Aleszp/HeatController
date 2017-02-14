#ifndef _UART_INTERRUPTS_H_
#define _UART_INTERRUPTS_H_

#define BAUD 9600U

#include <stdint.h>

#include "max6675.h"
#include "uart.h"
#include "global_variables.h"
#include "spi.h"

ISR (USART_RXC_vect) 
{
	//PORTA|=(1<<LED);
	RXBuf[rxindex] = UDR;
	rxindex++;
	if(RXBuf[0]==0xFF)
	{
		flags0|=(1<<PRINT_TEMP);
		rxindex=0;
		RXBuf[0]=0;
	}
	if(RXBuf[0]==0xEE)
	{
		flags0|=(1<<PRINT_AV);
		rxindex=0;
		RXBuf[0]=0;
	}
	if(RXBuf[0]==0xDD&&rxindex>2)
	{
		temp_desired=RXBuf[2];
		temp_desired|=(RXBuf[1]<<8);
		rxindex=0;
		RXBuf[0]=0;
		RXBuf[1]=0;
		RXBuf[2]=0;
	}
	if((RXBuf[0]!=0x99)&&(RXBuf[0]!=0xDD)&&(rxindex>0))
	{
		RXBuf[rxindex]=0;
		rxindex--;
	}
	if(RXBuf[0]==0xCC)
	{
		flags0|=(1<<PRINT_DES);
		rxindex=0;
		RXBuf[0]=0;
	}
	if(RXBuf[0]==0xBB)
	{
		flags0|=(1<<PRINT_K);
		rxindex=0;
		RXBuf[0]=0;
	}
	if(RXBuf[0]==0xAA)
	{
		flags0|=(1<<PRINT_PID);
		rxindex=0;
		RXBuf[0]=0;
	}
	if(RXBuf[0]==0x99&&rxindex>1)
	{
		ocr_index=RXBuf[1];
		rxindex=0;
		RXBuf[0]=0;
		RXBuf[1]=0;
		if(ocr_index<20)
		{
			tccr_index=0;
			return;
		}
	
		if(ocr_index>229)
		{
			tccr_index=2;
			return;
		}
		tccr_index=1;
	}	
	
	if(rxindex>=16)
		rxindex=0;
		
	//PORTA&=~(1<<LED);
}

ISR (USART_UDRE_vect) 
{
	//PORTA|=(1<<LED);					//zmień stan LEDa by zasygnalizować użytkownikowi, że "układ działa"
	while(!(UCSRA&(1<<UDRE)));
	if (txindex>0)
	{
		if(TXBuf[txindex-1]!=0x00)
			UDR=TXBuf[txindex-1];
		else
			UDR='\n';
		TXBuf[txindex-1]=0;
		txindex--;
	}
	else 
	{ 
		UCSRB &= ~(1<<UDRIE); 
	}
	//PORTA^=(1<<LED);					//zmień stan LEDa by zasygnalizować użytkownikowi, że "układ działa" 
}

#endif
