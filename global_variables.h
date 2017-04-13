#ifndef _GLOBAL_VARIABLES_H_
#define _GLOBAL_VARIABLES_H_

#include <stdint.h>

//Składowa proporcjonalna do zadanej temperatury
extern int8_t Kc;
//PID
extern int8_t Kp;				//mnożnik członu proporcjonalnego
extern int8_t Ki;				//mnożnik członu całkującego
extern int8_t Kd;				//mnożnik członu różniczkującego

//timer0
extern const uint8_t ocr[256];
extern const uint8_t tccr[3];
extern volatile uint8_t ocr_index;		//indeks wybierający z tablicy ocr co wpisać do rejestru OCR0
extern volatile uint8_t tccr_index;		//indeks wybierający z tablicy tccr co wpisać do rejestru TCCR0

extern volatile int32_t time;

//uart
extern volatile char RXBuf[16];			//tablica odebranych znaków
extern volatile char TXBuf[32];			//tablica znaków do wysłania
extern volatile uint8_t rxindex;		//indeks znaku do odebrania
extern volatile uint8_t txindex;		//indeks znaku do wysłania

//max6675
extern volatile int16_t temp[64];		//tablica ostatnich 64 zmierzonych temperatur
extern volatile int16_t temp_id;		//indeks ostatnio zmierzonej temperatury
extern volatile int16_t temp_av;		//średnia z ostatnich 64 pomiarów temperatury
extern volatile int16_t temp_desired;	//temperatura docelowa
extern volatile int16_t temp_deriv;		//pochodna temperatury po czasie
extern volatile int32_t temp_iteg;		//całka z temperatury w funkcji czasu

//flagi
extern volatile uint8_t flags0;			//flagi (w razie potrzeby zostaną utworzone kolejne flagi 8 bitowe, 
extern volatile uint8_t flags1;			//co jest wydajniejsze niż jedna flaga 8*n bitowa)

//definicje przesunięć bitowych flag
#define MEASURE      0
#define PRINT_TEMP   1
#define PRINT_AV     2
#define PRINT_DES    3
#define PRINT_K 	 4
#define PRINT_PID 	 5
#define PRINT_NO_TC  6
#define ERROR_NO_TC  7
#define COUNT_PID    0
#define FIRST_MINUTE 1
#define TRIGGER		 2



#endif
