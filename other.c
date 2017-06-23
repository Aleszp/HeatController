#include <util/delay.h>
#include <avr/wdt.h> 
#include <avr/interrupt.h>

#include "other.h"


void delay40(uint8_t times)
{
	for(;times>0;times--)
		_delay_us(40);
}

uint16_t round16(uint16_t src,uint8_t bits)
{
	src+=(1<<(bits-1));						//zwiększ część pomijalną o 1/2, w ten sposób 
	return (src>>bits);						//obcinając ją, zaokrąglamy do bliższej wartości, nie w dół
}

void restart(void)
{
	cli();							//wyłącz obsługę przerwań (by uniknąć problemów z np komunikacją z PC)
	wdt_enable(WDTO_15MS);			//włącz zegar watchdog z najkrótszym okresem (ok 15ms)
	while(1) {}						//utknij w nieskończonej pętli aż do restartu watchdoga
}
