###############################################################################
# Makefile for the project linetarcer_step
###############################################################################

## General Flags
PROJECT = linetarcer_step
MCU = atmega128
TARGET = linetarcer_step.elf
CC = avr-gcc

CPP = avr-g++

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -std=gnu99     -DF_CPU=16000000UL -Os -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d 

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS +=  -Wl,-Map=linetarcer_step.map


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Objects that must be built in order to link
OBJECTS = linetarcer_step.o linetracer_ADC.o linetracer_MOTOR.o linetracer_UART.o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) linetarcer_step.hex linetarcer_step.eep linetarcer_step.lss size

## Compile
linetarcer_step.o: ../linetarcer_step.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

linetracer_ADC.o: ../linetracer_ADC.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

linetracer_MOTOR.o: ../linetracer_MOTOR.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

linetracer_UART.o: ../linetracer_UART.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

##Link
$(TARGET): $(OBJECTS)
	 $(CC) $(LDFLAGS) $(OBJECTS) $(LINKONLYOBJECTS) $(LIBDIRS) $(LIBS) -o $(TARGET)

%.hex: $(TARGET)
	avr-objcopy -O ihex $(HEX_FLASH_FLAGS)  $< $@

%.eep: $(TARGET)
	-avr-objcopy $(HEX_EEPROM_FLAGS) -O ihex $< $@ || exit 0

%.lss: $(TARGET)
	avr-objdump -h -S $< > $@

size: ${TARGET}
	@echo
	@avr-size -C --mcu=${MCU} ${TARGET}

## Clean target
.PHONY: clean
clean:
	-rm -rf $(OBJECTS) linetarcer_step.elf dep/* linetarcer_step.hex linetarcer_step.eep linetarcer_step.lss linetarcer_step.map


## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)

