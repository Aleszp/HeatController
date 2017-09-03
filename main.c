#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include "init.h"
#include "interrupts.h"
#include "max6675.h"
#include "other.h"
#include "pid.h"
#include "pinout.h"
#include "spi.h"
#include "uart.h"

int main(void)
{
	init(11,1); 							//11 - 57.6k baud RS232 (przy 11.0592MHz, wartość z noty katalogowej), 1 - pomiar temperatury co 1 sekundę
											//51 dla baud = 9600, 12 dla baud=38.4k (F_Osc = 8MHz)
	_delay_ms(100);
	while(1)
	{
		if(flags0&(1<<MEASURE))				//jeśli ustawiono flagę pomiaru temperatury (ustawiane jest w przerwaniu co 1 sekundę)
		{
			update();						//mierz temperaturę, oblicz średnią temperaturę
			flags0&=~(1<<MEASURE);			//odwołaj flagę pomiaru
			flags0|=(1<<PRINT_TEMP);
		}
		
		if(flags0&(1<<PRINT_TEMP))			//jeśli ustawiono flagę wyświetlenia aktualnej temperatury (ustawiana głównej pętli programu co 1 sekundę)
		{
			USART_Append_To_Buffer_uint32(time,1,1);
			USART_Append_To_Buffer_int16(temp[temp_id],1,1);
			USART_Append_To_Buffer_uint8(ocr_index,0,0);
			flags0&=~(1<<PRINT_TEMP);		//odwołaj flagę wyświetlenia aktualnej temperatury
		}
		
		if(flags0&(1<<PRINT_AV))			//jeśli ustawiono flagę wyświetlenia średniej temperatury (ustawiana w przerwaniu na żądanie użytkownika)
		{
			USART_Append_To_Buffer(PSTR("AV  "),4,1);
			disp_Temp(temp_av,1);			//wyświetl średnią temperaturę
			USART_Append_To_Buffer(PSTR("IT  "),4,1);
			disp_Temp(temp_iteg,1);		
			USART_Append_To_Buffer(PSTR("DER "),4,1);
			disp_Temp(temp_deriv,0);		
			flags0&=~(1<<PRINT_AV);			//odwołaj flagę wyświetlenia średniej temperatury
		}
		
		if(flags0&(1<<PRINT_DES))			//jeśli ustawiono flagę wyświetlenia zadanej temperatury (ustawiana w przerwaniu na żądanie użytkownika)
		{
			USART_Append_To_Buffer(PSTR("DES "),4,1);
			disp_Temp(temp_desired,0);		//wyświetl zadaną temperaturę
			flags0&=~(1<<PRINT_DES);		//odwołaj flagę wyświetlenia zadanej temperatury
		}
		
		if(flags1&(1<<TRIGGER)&&!(flags0&(1<<ERROR_NO_TC)))
		{
			delay40(pgm_read_byte(&ocr[ocr_index]));
			PORTD|=(1<<MOC3021);
			PORTA|=(1<<LED);
			delay40(pgm_read_byte(&ocr[ocr_index])<41?100:25);
			PORTD&=~(1<<MOC3021);
			flags1&=~(1<<TRIGGER);
		}
		if(flags0&(1<<ERROR_NO_TC))
		{
			PORTD&=~(1<<MOC3021);
			PORTA&=~(1<<LED);
		}
		
		if(flags0&(1<<PRINT_K))				//jeśli ustawiono flagę wyświetlenia współczynników PID (ustawiana w przerwaniu na żądanie użytkownika)
		{
			USART_Append_To_Buffer(PSTR("Kp "),3,1);
			USART_Append_To_Buffer_int8(Kp,1,0);
			USART_Append_To_Buffer(PSTR("Ki "),3,1);
			USART_Append_To_Buffer_int8(Ki,1,0);
			USART_Append_To_Buffer(PSTR("Kd "),3,1);
			USART_Append_To_Buffer_int8(Kd,0,0);
			flags0&=~(1<<PRINT_K);			//odwołaj flagę wyświetlenia parametrów PID
		}
		
		if(flags0&(1<<PRINT_PID))			//jeśli ustawiono flagę wyświetlenia parametrów PID (ustawiana w przerwaniu na żądanie użytkownika)
		{
			USART_Append_To_Buffer(PSTR("ocr_index "),10,1);
			USART_Append_To_Buffer_uint8(ocr_index,1,0);
			USART_Append_To_Buffer(PSTR("ocr0 "),5,1);
			USART_Append_To_Buffer_uint8(pgm_read_byte(&ocr[ocr_index]),0,0);
			flags0&=~(1<<PRINT_PID);		//odwołaj flagę wyświetlenia parametrów PID
		}
		
		if(flags0&(1<<PRINT_NO_TC))			//jeśli ustawiono flagę wyświetlenia parametrów PID (ustawiana w przerwaniu na żądanie użytkownika)
		{
			USART_Append_To_Buffer(PSTR("ERR NO TC\n"),10,0);
			flags0&=~(1<<PRINT_NO_TC);		//odwołaj flagę wyświetlenia parametrów PID
		}
		
		if(flags1&(1<<COUNT_PID))			//jeśli ustawiono flagę liczenia parametrów PID
		{
			calculate_PID();				//policz parametry PID
			flags1&=~(1<<COUNT_PID);		//odwołaj flagę obliczania parametrów PID
		}
		if(flags1&(1<<RESET))				//jeśli ustawiono flagę programowego restartu
		{
			restart();
		}
		if(flags1&(1<<HELP))				//jeśli ustawiono flagę wyświetlenia pomocy
		{
			USART_Append_To_Buffer(PSTR("Znane rozkazy:\n"),15,1);
			USART_Append_To_Buffer(PSTR("A - wysw sr temp\n"),17,1);
			USART_Append_To_Buffer(PSTR("Txxx - ustaw temp = xxx\n"),24,1);
			USART_Append_To_Buffer(PSTR("Pxxx - ustaw Kp = xxx\n"),22,1);
			USART_Append_To_Buffer(PSTR("Cxxx - ustaw Ki = xxx\n"),22,1);
			USART_Append_To_Buffer(PSTR("Qxxx - ustaw Kd = xxx\n"),22,1);
			USART_Append_To_Buffer(PSTR("Mxxx - ustaw mianownik = xxx\n"),29,1);
			USART_Append_To_Buffer(PSTR("D - wysw temp docel\n"),20,1);
			USART_Append_To_Buffer(PSTR("R - restart\n"),12,1);
			USART_Append_To_Buffer(PSTR("? - wysw pomoc\n"),15,0);
			flags1&=~(1<<HELP);				//odwołaj flagę wyświetlenia pomocy
		}
	}
}
