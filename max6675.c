#include <stddef.h>
#include <stdint.h>
#include <avr/io.h>

#include "max6675.h"
#include "spi.h"
#include "uart.h"
#include "pinout.h"
#include "global_variables.h"

int16_t get_Temp(void)
{
	int16_t buff=(SPI_MasterReceiveWord());
	if(buff&0b100)
	{
		flags0|=(1<<ERROR_NO_TC);
		return 0xFFFF;
	}
	flags0&=~(1<<ERROR_NO_TC);
	return (buff>>3);
}

void measure_Temp(void)
{
	//PORTA|=(1<<LED);					//zmień stan LEDa by zasygnalizować użytkownikowi, że "układ działa"
	int16_t buff=(SPI_MasterReceiveWord());
	if(buff&0b100)
	{
		flags0|=(1<<ERROR_NO_TC)|(1<<PRINT_NO_TC);
		return;
	}
	flags0&=~(1<<ERROR_NO_TC);
	buff>>=3;
	temp_id++;
	if(temp_id>=64)
	{
		temp_id=0;
		if(flags1&(1<<FIRST_MINUTE))
			flags1&=~(1<<FIRST_MINUTE);
	}
	temp[temp_id]=buff;
	temp_iteg+=temp_desired-buff;
	//PORTA&=~(1<<LED);					//zmień stan LEDa by zasygnalizować użytkownikowi, że "układ działa"
}

void disp_Temp(int16_t temp_, uint8_t wait)
{
	char buffer[12];
	if(flags0&(1<<ERROR_NO_TC))
	{
		buffer[0]='E';
		buffer[1]='R';
		buffer[2]='R';
		buffer[3]=' ';
		buffer[4]='N';
		buffer[5]='O';
		buffer[6]=' ';
		buffer[7]='T';
		buffer[8]='C';
		buffer[9]='\n';
	}
	else
	{
		int8_t tmp=0;
		buffer[0]=temp_>=0?'+':'-';
		temp_*=temp_>=0?1:-1;
		int8_t fract=(temp_&0b11)*25; //ułamkowa część (w ćwiartkach stopni celsjusza)
		
		temp_>>=2;
		
		
		tmp=temp_/10000;
		buffer[1]='0'+tmp;
		temp_-=10000*tmp;
		tmp=temp_/1000;
		buffer[2]='0'+tmp;
		temp_-=1000*tmp;
		tmp=temp_/100;
		buffer[3]='0'+tmp;
		temp_-=100*tmp;
		tmp=temp_/10;
		buffer[4]='0'+tmp;
		temp_-=10*tmp;
		buffer[5]='0'+temp_;
		buffer[6]='.';
		tmp=fract/10;
		fract-=tmp*10;
		buffer[7]='0'+tmp;
		buffer[8]='0'+fract;
		buffer[9]=' ';
		buffer[10]='C';
		buffer[11]='\n';
	}
	USART_Append_To_Buffer(buffer,12,wait);
}

void update(void)
{ 
	//PORTA|=(1<<LED);
	measure_Temp();						//zmierz temperaturę
	calculate_Av_Temp();				//policz średnią temperaturę
	//PORTA&=~(1<<LED);
}

void calculate_Av_Temp(void)
{
	int32_t buff=0;
	
	if(flags1&(1<<FIRST_MINUTE))
	{
		uint8_t tmp_quantity=temp_id;
		for(uint8_t i=0;i<tmp_quantity;i++)
		{
			buff+=temp[i];
		}
		if(tmp_quantity<8)
		{
			temp_deriv=(temp[temp_id]-temp[0]);
			temp_deriv/=tmp_quantity;
		}
		else
		{
			temp_deriv=(temp[temp_id]-temp[temp_id-8]);
			temp_deriv>>3;
		}
		
		temp_av=buff/tmp_quantity;
		return;
	}
	for(uint8_t i=0;i<64;i++)
	{
		buff+=temp[i];
	}
	temp_av=(buff>>6);
	uint8_t tmp=temp_id-8;
	if(tmp>=64)
		tmp+=64;
	temp_deriv=(temp[temp_id]-temp[tmp]);
	temp_deriv>>3;
}
