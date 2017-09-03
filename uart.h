#ifndef _UART_H_
#define _UART_H_

#include <stdint.h>

void USART_Init(uint16_t baud);
void USART_Append_To_Buffer(const char const* src, uint8_t len, uint8_t wait);
void USART_Append_To_Buffer_int8(int8_t src, uint8_t wait, uint8_t space);
void USART_Append_To_Buffer_int16(int16_t src, uint8_t wait, uint8_t space);
void USART_Append_To_Buffer_uint8(uint8_t src, uint8_t wait, uint8_t space);
void USART_Append_To_Buffer_uint32(int32_t src, uint8_t wait, uint8_t space);

void inline uart_disable_send(void){UCSRB &= ~(1<<UDRIE);} 	//zablokuj wysyłanie na czas tej operacji
void inline uart_enable_send(void){UCSRB |= (1<<UDRIE);}  	//odblokuj wysyłanie

#endif
