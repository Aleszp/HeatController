#ifndef _INTERRUPTS_H_
#define _INTERRUPTS_H_

#include <stdint.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stddef.h>

#include "global_variables.h"
#include "pinout.h"
#include "uart_interrupts.h"


ISR(TIMER1_COMPA_vect)
{
	flags0|=(1<<MEASURE);
	flags1|=(1<<COUNT_PID);
}

ISR(INT1_vect)
{
	PORTA&=~(1<<LED);
	PORTD&=~(1<<MOC3021);
	TCCR0=(1<<WGM01);
	TCNT0=0x00;								//wyzeruj zegar
	if(ocr_index==0)						//jeśli potrzebna moc = 0W
	{
		return;								//zakończ przerwanie nie wyzwalając triaka		
	}
		
	if(ocr_index==255)						//jeśli potrzeba 100% mocy
	{
		PORTD|=(1<<MOC3021);				//wyzwól triak natychmiast
		flags1|=(1<<TRIGGER);
		return;								//i zakończ przerwanie
	}	
	flags1|=(1<<TRIGGER);					//w pozostałych przypadkach

#endif
