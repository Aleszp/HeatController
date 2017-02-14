#ifndef SPI_H_
#define SPI_H_

#include <stdint.h>

void SPI_MasterInit(void);
void SPI_MasterTransmit(uint8_t byte);
uint8_t SPI_MasterReceiveByte_u(void);
int8_t SPI_MasterReceiveByte(void);
uint16_t SPI_MasterReceiveWord_u(void);
int16_t SPI_MasterReceiveWord(void);

#endif
