#include <stdint.h>
#include <avr/interrupt.h>
#include <avr/io.h>


#include "global_variables.h"
#include "init.h"
#include "pinout.h"
#include "spi.h"
#include "uart.h"

#define PRESKALER1 256

void init(uint16_t baud, uint16_t freq)
{
	Ports_Init();
	INT1_Init();
	SPI_MasterInit();
	USART_Init(baud);				//51 dla baud = 9600, 12 dla baud=38.4k (F_Osc = 8MHz)
	TIMER1_Init(freq);
	USART_Append_To_Buffer("INIT FINISHED\n",14,0);
	
	flags1|=(1<<FIRST_MINUTE);
	sei();
}

void Ports_Init(void)
{
	DDRA=0xFF;
	DDRB=0xFF;
	DDRC=0xFF;
	DDRD=0xFF&~(1<<SPH602A);		//nasłuchuj na SPH
	PORTA=0xFF;
	PORTA&=~(1<<LED);
	
	PORTD=0xFF&(~(1<<MOC3021));		//podciągnij rezystory, stan niski na MOC3021
}

void INT1_Init(void)
{
	MCUCR|=(1<<ISC11|1<<ISC10);		//Przerwanie INT1 dla zbocza narastającego na PD3
	GICR|=(1<<INT1);
}

void TIMER1_Init(uint16_t freq)		//freq - częstotliwość przerywania 
{
	TCCR1B|=(1<<WGM12);				//tryb CTC
	TCCR1B|=(1<<CS12); 				//preskaler 256
	
	OCR1A=F_CPU/(freq*PRESKALER1)-1;
	
	TIMSK|=(1<<OCIE1A);
}
