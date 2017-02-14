#include <util/delay.h>
#include "other.h"


void delay40(uint8_t times)
{
	for(;times>0;times--)
		_delay_us(40);
}

uint16_t round16(uint16_t src,uint8_t bits)
{
	src+=(1<<bits);
	return (src>>bits);
}
