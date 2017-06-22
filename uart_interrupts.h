#ifndef _UART_INTERRUPTS_H_
#define _UART_INTERRUPTS_H_

#include <stdint.h>

#include "max6675.h"
#include "uart.h"
#include "global_variables.h"
#include "spi.h"

ISR (USART_RXC_vect) 
{
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
		temp_iteg=0;
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
	if(RXBuf[0]==0x11)
	{
		flags1|=(1<<RESET);
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
	
	if(rxindex>=16)					//jeśli przepełni się bufor
		rxindex=0;					//opróżnij go (zezwól na jego nadpisywanie od początku)
}

ISR (USART_UDRE_vect) 
{
	while(!(UCSRA&(1<<UDRE)));
	if (txindex>0)					//jeśli są dane do wysłania
	{
		if(TXBuf[txindex-1]!=0x00) 	//jeśli to nie jest znak "końca linii"
			UDR=TXBuf[txindex-1];  	//prześlij go dalej
		else
			UDR='\n';				//wyślij (prawdziwy) znak końca linii
		TXBuf[txindex-1]=0;
		txindex--;
	}
	else 							//jeśli już nie ma nic do wysłania
	{
		UCSRB &= ~(1<<UDRIE);		//zablokuj to przerwanie 
	}
}

#endif
