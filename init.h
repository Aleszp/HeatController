#ifndef _INIT_H_
#define _INIT_H_

void init(uint16_t baud, uint16_t freq);
void Ports_Init(void);
void INT1_Init(void);
//void TIMER0_Init(void);
void TIMER1_Init(uint16_t freq);


#endif
