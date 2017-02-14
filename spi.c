#include <avr/io.h>
#include <stdint.h>

#include "spi.h"
#include "pinout.h"

void SPI_MasterInit(void)
{
	DDRB |= 0xFF;							//ustaw wyjście dla linii MOSI, SCK i SS
	DDRB &= ~(1<<MISO);						//ustaw wejście dla linii MISO
	PORTB=0xFF;								//podciągnij rezystory
	
	SPCR|=(1<<SPE)|(1<<MSTR); 				//aktywacja magistrali SPI, tryb Master, częstotliwość zegara SPI (2MHz) = 1/4 zegara oscylatora (8MHz) 
	SPCR|=(1<<CPHA);						//odczyt na opadającym zboczu zegara
	
	SPCR&= ~(1<<DORD); 						//najpierw MSB, potem LSB
}

int8_t SPI_MasterReceiveByte(void)
{
	SPDR = 0xFF;							//Wyślij bajt
	while(!(SPSR & (1<<SPIF)));				//Czekaj aż zostanie wysłany
	return SPDR;							//Zwróć odebrany bajt
}

uint8_t SPI_MasterReceiveByte_u(void)
{
	SPDR = 0xFF;							//Wyślij bajt
	while(!(SPSR & (1<<SPIF)));				//Czekaj aż zostanie wysłany
	return SPDR;							//Zwróć odebrany bajt
}

int16_t SPI_MasterReceiveWord(void)
{
	int16_t buffer=0x0000;					//stwórz bufor dla odebranego słowa
	PORTB&=~(1<<SS);						//rozpocznij komunikację z urządzeniem (zatrzymaj przetwornik w MAX6675)
	buffer|=(SPI_MasterReceiveByte()<<8);	//pobierz starszy bajt
	buffer|=(SPI_MasterReceiveByte_u());	//pobierz młodszy bajt
	PORTB|=(1<<SS);							//zatrzymaj komunikację z urządzeniem (wznów pracę przetwornika w MAX6675)
	return buffer;							//zwróć zawartość dwubajtowego bufora
}

uint16_t SPI_MasterReceiveWord_u(void)
{
	uint16_t buffer=0x0000;					//stwórz bufor dla odebranego słowa
	PORTB&=~(1<<SS);						//rozpocznij komunikację z urządzeniem (zatrzymaj przetwornik w MAX6675)
	buffer|=(SPI_MasterReceiveByte()<<8);	//pobierz starszy bajt
	buffer|=(SPI_MasterReceiveByte());		//pobierz młodszy bajt
	PORTB|=(1<<SS);							//zatrzymaj komunikację z urządzeniem (wznów pracę przetwornika w MAX6675)
	return buffer;							//zwróć zawartość dwubajtowego bufora
}
