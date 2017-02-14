#ifndef _MAX6675_H_
#define _MAX6675_H_

#include <stdint.h>

int16_t get_Temp(void);
void measure_Temp(void);
void disp_Temp(int16_t temp_, uint8_t wait);
void update(void);
void calculate_Av_Temp(void);

#endif

