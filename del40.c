#include <util/delay.h>
#include "del40.h"


void delay40(uint8_t times)
{
	for(;times>0;times--)
		_delay_us(40);
}
