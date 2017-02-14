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
/*
ISR(TIMER0_COMP_vect)
{
	TCCR0=(1<<WGM01); 						//zatrzymaj swój zegar 
	TCNT0=0x00;								//i wyzeruj się
	
	if((PORTD&(1<<MOC3021))&&1) 				//jeśli wyjście na MOC3021 LO
	{
		PORTD|=(1<<MOC3021);				//ustaw HI
		PORTA|=(1<<LED);
		OCR0=199;							//ustaw oczekiwanie na długość potrzebnego impulsu (200us będzie wystarczające)
		TCCR0=(1<<WGM01)|(1<<CS01);			//włącz zegar z dzielnikiem 8
		return;								//i zakończ przerwanie
	}
											//jeśli wyjście na MOC3021 HI
	PORTA&=~(1<<LED);										
	PORTD&=~(1<<MOC3021);					//ustaw je LO
											//i zakończ przerwanie
}
*/
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
		
		//OCR0=199;							//ustaw oczekiwanie na długość potrzebnego impulsu (200us będzie wystarczające)
		//TCCR0=(1<<CS01)|(1<<WGM01);			//włącz zegar z dzielnikiem 8
		return;								//i zakończ przerwanie
	}	
	flags1|=(1<<TRIGGER);					//w pozostałych przypadkach:
											//włącz licznik0 (ustaw jego czas na podstawie wcześniej obliczonych wartości, 
	//OCR0=pgm_read_byte(&ocr[ocr_index]);	//ustaw wartość porównania, ocr_index wybiera opóźnienie (ocr_index:<0;255>)
	//TCCR0=pgm_read_byte(&tccr[tccr_index]);	//włącz zegar, tccr_index wybiera preskaler (tccr_index:<0;2>)
}

#endif
