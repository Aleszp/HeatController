#include <avr/pgmspace.h>
#include <stdint.h>

#include "global_variables.h"

//PID
volatile int8_t Kp=32;				//mnożnik członu proporcjonalnego
volatile int8_t Ki=64;				//mnożnik członu całkującego
volatile int8_t Kd=0;				//mnożnik członu różniczkującego

//timer0
	/*Wartości do wpisania do rejestru OCR w zależności od potrzebnego opóźnienia (pierwsza i ostatnia wartość (0) są specjalne, dla 100% mocy reakcja
	/ jest natychmiastowa - przerwanie zewnętrzne wyzwala triaka, dla 0% mocy triak nie jest wyzwalany (pozostają tu dla zgodności, 
	/ bo "procent bajtowy" (1/255) mocy może zostać użyty jako indeks tablicy bez wykonywania na nim zbędnych operacji. */
const uint8_t ocr[256] PROGMEM =
{
	250, 240, 236, 233, 230, 228, 225, 224, 222, 220, 218, 217, 215, 214, 212, 211, 210, 208, 207, 206, 205, 204, 203, 201, 200, 
	199, 198, 197, 196, 195, 194, 193, 192, 191, 191, 190, 189, 188, 187, 186, 185, 184, 183, 183, 182, 181, 180, 179, 179, 178, 
	177, 176, 175, 175, 174, 173, 172, 172, 171, 170, 169, 169, 168, 167, 166, 166, 165, 164, 164, 163, 162, 162, 161, 160, 159, 
	159, 158, 157, 157, 156, 155, 155, 154, 153, 153, 152, 151, 151, 150, 149, 149, 148, 147, 147, 146, 146, 145, 144, 144, 143, 
	142, 142, 141, 140, 140, 139, 138, 138, 137, 137, 136, 135, 135, 134, 133, 133, 132, 132, 131, 130, 130, 129, 128, 128, 127, 
	127, 126, 125, 125, 124, 123, 123, 122, 122, 121, 120, 120, 119, 118, 118, 117, 117, 116, 115, 115, 114, 113, 113, 112, 112, 
	111, 110, 110, 109, 108, 108, 107, 106, 106, 105, 104, 104, 103, 103, 102, 101, 101, 100, 99, 99, 98, 97, 97, 96, 95, 95, 94, 
	93, 93, 92, 91, 91, 90, 89, 88, 88, 87, 86, 86, 85, 84, 84, 83, 82, 81, 81, 80, 79, 78, 78, 77, 76, 75, 75, 74, 73, 72, 71, 71, 
	70, 69, 68, 67, 67, 66, 65, 64, 63, 62, 61, 60, 59, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 47, 46, 45, 44, 43, 42, 40, 39, 
	38, 36, 35, 33, 32, 30, 28, 26, 25, 22, 20, 17, 14, 10, 0
};

/*{
	0,74,73, 72, 71, 70, 69, 69, 68, 68, 67, 67, 66, 66, 65, 65, 65, 64, 64, 63, 255, 254, 252, 251, 249, 248, 247, 246, 244, 
	243, 242, 241, 239, 238, 237, 236, 235, 234, 233, 232, 230, 229, 228, 227, 226, 225, 224, 223, 222, 221, 220, 219, 218, 217, 
	216, 215, 214, 214, 213, 212, 211, 210, 209, 208, 207, 206, 205, 204, 204, 203, 202, 201, 200, 199, 198, 197, 197, 196, 195, 
	194, 193, 192, 192, 191, 190, 189, 188, 187, 187, 186, 185, 184, 183, 183, 182, 181, 180, 179, 178, 178, 177, 176, 175, 174, 
	174, 173, 172, 171, 171, 170, 169, 168, 167, 167, 166, 165, 164, 163, 163, 162, 161, 160, 160, 159, 158, 157, 156, 156, 155, 
	154, 153, 153, 152, 151, 150, 149, 149, 148, 147, 146, 145, 145, 144, 143, 142, 142, 141, 140, 139, 138, 138, 137, 136, 135, 
	134, 134, 133, 132, 131, 130, 130, 129, 128, 127, 126, 126, 125, 124, 123, 122, 121, 121, 120, 119, 118, 117, 116, 116, 115, 
	114, 113, 112, 111, 110, 110, 109, 108, 107, 106, 105, 104, 103, 102, 102, 101, 100, 99, 98, 97, 96, 95, 94, 93, 92, 91, 90, 
	89, 88, 87, 86, 85, 84, 83, 82, 81, 80, 79, 78, 77, 76, 75, 73, 72, 71, 70, 69, 67, 66, 65, 64, 252, 247, 242, 236, 231, 225, 
	219, 213, 207, 200, 194, 187, 180, 173, 165, 158, 149, 141, 131, 122, 111, 99, 85, 70, 49, 0
};*/	
	/*Wartości mnożników do ustawienia w TCCR0 dla 3 LSB wybierające dzielnik częstotliwości (1/64, 1/256, 1/1024).*/
const uint8_t tccr[3] PROGMEM = {(1<<WGM01)|(CS01<<1)|(CS00<<1),(1<<WGM01)|(CS02<<1),(1<<WGM01)|(CS01<<1)|(CS02<<1)};
volatile uint8_t ocr_index=0;		//indeks wybierający z tablicy ocr co wpisać do rejestru OCR0
volatile uint8_t tccr_index=0;		//indeks wybierający z tablicy tccr co wpisać do rejestru TCCR0

volatile int32_t time=0;

//uart
volatile char RXBuf[16]=			//tablica odebranych znaków
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
volatile char TXBuf[32]=			//tablica znaków do wysłania
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
volatile uint8_t rxindex=0;			//indeks znaku do odebrania
volatile uint8_t txindex=0;			//indeks znaku do wysłania

//max6675
volatile int16_t temp[64]=			//tablica ostatnich 64 zmierzonych temperatur
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};		
volatile int16_t temp_id=0;			//indeks ostatnio zmierzonej temperatury
volatile int16_t temp_av=0;			//średnia z ostatnich 64 pomiarów temperatury
volatile int16_t temp_desired=0;	//temperatura docelowa
volatile int16_t temp_deriv=0;		//pochodna temperatury po czasie
volatile int32_t temp_iteg=0;		//całka z temperatury w funkcji czasu

//flagi
volatile uint8_t flags0=0;			//flagi (w razie potrzeby zostaną utworzone kolejne flagi 8 bitowe)
volatile uint8_t flags1=0;
