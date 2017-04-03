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
	buffer+=(temp_desired-temp[temp_id])*Kp;
	buffer+=(temp_iteg)*Ki;
	//buffer+=(temp_deriv)*Kd;
	//sei();
	//USART_Append_To_Buffer_int16((int16_t)buffer,1);
	//cli();
	buffer/=16;
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
