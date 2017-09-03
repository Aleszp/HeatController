#include <stdint.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/wdt.h> 
#include <avr/pgmspace.h>

#include "global_variables.h"
#include "init.h"
#include "pinout.h"
#include "spi.h"
#include "uart.h"

#define PRESKALER1 256

void init(uint16_t baud, uint16_t freq)
{
	wdt_disable();					//(na wszelki wypadek) zablokuj zegar watchdog
	Ports_Init();
	INT1_Init();
	SPI_MasterInit();
	USART_Init(baud);				//51 dla baud = 9600, 12 dla baud=38.4k (F_Osc = 8MHz)
	TIMER1_Init(freq);
	flags1|=(1<<FIRST_MINUTE);		//Ustaw flagę pierwszej minuty (ważne dla funkcji liczących całkę z temperatury)
	sei();
	USART_Append_To_Buffer(PSTR("INICJALIZACJA ZAKONCZONA\n"),25,1);	//poinformuj użytkownika o zakończonej inicjalizacji
	USART_Append_To_Buffer(PSTR("? - wyswietla pomoc\n"),20,1);
	USART_Append_To_Buffer(PSTR("\tt/s,\tT/C,\tP[0-255]\n"),20,0);

}

void Ports_Init(void)
{
	DDRA=0xFF;						//ustaw wszystkie porty jako wyjścia
	DDRB=0xFF;
	DDRC=0xFF;
	DDRD=0xFF&~(1<<SPH602A);		//poza pinem na którym będzie nasłuchiwać sygnału od układu SPH602A
	PORTA=0xFF;						//ustaw stan wysoki na porcie A
	PORTA&=~(1<<LED);				//ale zgaś LEDa
	
	PORTD=0xFF&(~(1<<MOC3021));		//podciągnij rezystory, stan niski na MOC3021
}

void INT1_Init(void)
{
	MCUCR|=(1<<ISC11|1<<ISC10);		//Przerwanie INT1 dla zbocza narastającego na PD3
	GICR|=(1<<INT1);				//Odblokuj przerwanie INT1
}

void TIMER1_Init(uint16_t freq)		//freq - częstotliwość przerywania 
{
	TCCR1B|=(1<<WGM12);				//tryb CTC
	TCCR1B|=(1<<CS12); 				//preskaler 256
	
	OCR1A=F_CPU/(freq*PRESKALER1)-1;//oblicz właściwą wartość rejestru OCR1A
	
	TIMSK|=(1<<OCIE1A);				//odblokuj timer1
}
