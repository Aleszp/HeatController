CC = avr-gcc
OBJCOPY = avr-objcopy
F_CPU = 11059200
FLAGS = -std=gnu99 -Wall -pedantic -Os -DF_CPU=$(F_CPU)UL
FLAGS += -funsigned-char
FLAGS += -funsigned-bitfields
FLAGS += -fpack-struct
FLAGS += -fshort-enums
FORMAT=ihex
TARGET=main

AVRDUDE = avrdude
MCU = atmega32
AVRDUDE_PORT = usb
AVRDUDE_PROGRAMMER = usbasp-clone
AVRDUDE_FLAGS = -p $(MCU) -P $(AVRDUDE_PORT) -c $(AVRDUDE_PROGRAMMER)
AVRDUDE_FLAGS += -E reset
AVRDUDE_WRITE_FLASH = -U flash:w:$(TARGET).hex

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -j .text -j .data -O $(FORMAT) $(TARGET).elf $(TARGET).hex
$(TARGET).eep: $(TARGET).elf
	$(OBJCOPY) -j .eeprom --set-section-flags=.eeprom="alloc,load" --change-section-lma .eeprom=0 -O $(FORMAT) $(TARGET).elf $(TARGET).eep
$(TARGET).elf: global_variables.o init.o max6675.o other.o pid.o spi.o uart.o $(TARGET).o 
	$(CC) -g -mmcu=$(MCU) $(TARGET).o global_variables.o init.o max6675.o other.o pid.o spi.o uart.o -o $(TARGET).elf
global_variables.o:	global_variables.c global_variables.h
	$(CC) -g $(FLAGS) -mmcu=$(MCU) -c global_variables.c -o global_variables.o
init.o:	init.c global_variables.h init.h pinout.h spi.h uart.h
	$(CC) -g $(FLAGS) -mmcu=$(MCU) -c init.c -o init.o	
max6675.o:	max6675.c global_variables.h max6675.h other.h pinout.h spi.h uart.h
	$(CC) -g $(FLAGS) -mmcu=$(MCU) -c max6675.c -o max6675.o
other.o:	other.c other.h
	$(CC) -g $(FLAGS) -mmcu=$(MCU) -c other.c -o other.o
pid.o:	pid.c global_variables.h pid.h
	$(CC) -g $(FLAGS) -mmcu=$(MCU) -c pid.c -o pid.o
spi.o:	spi.c spi.h pinout.h
	$(CC) -g $(FLAGS) -mmcu=$(MCU) -c spi.c -o spi.o
uart.o:	uart.c uart.h global_variables.h
	$(CC) -g $(FLAGS) -mmcu=$(MCU) -c uart.c -o uart.o			
$(TARGET).o:	$(TARGET).c interrupts.h uart_interrupts.h global_variables.h pinout.h init.h max6675.h other.h pid.h spi.h uart.h
	$(CC) -g $(FLAGS) -mmcu=$(MCU) -c $(TARGET).c -o $(TARGET).o

program: $(TARGET).hex $(TARGET).eep
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_WRITE_FLASH) $(AVRDUDE_WRITE_EEPROM)

clean:
	rm -f *.o
	rm -f *.hex
	rm -f *.elf
	rm -f *.eep
