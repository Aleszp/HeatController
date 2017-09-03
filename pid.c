#include <stdint.h>
#include <avr/interrupt.h>

#include "global_variables.h"
#include "pid.h"

void calculate_PID(void)
{
	cli();
	if(temp_desired+(100)<temp[temp_id]||temp_desired<20)
	{
		ocr_index=0;
		sei();
		return;
	}
	
	int32_t buffer=0;
	buffer+=(temp_desired)*Kc;
	buffer+=(temp_desired-temp[temp_id])*Kp;
	buffer+=(temp_iteg)*Ki;
	buffer+=(temp_deriv)*Kd;
	buffer/=denominator;		//teoretycznie zamiast dzielenia mógłbym zastosować przesunięcie bitowe, ale przyrost wydajności byłby niewielki w porównaniu z ograniczeniem do dzielenia wyłącznie przez potęgi liczby 2
	if(buffer>255)
	{
		ocr_index=255;
		sei();
		return;
	}
	
	if(buffer>=0)
		ocr_index=buffer;
	else
		ocr_index=0;
	sei();
}
