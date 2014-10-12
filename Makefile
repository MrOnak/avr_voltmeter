# WinAVR cross-compiler toolchain is used here
CC = avr-gcc
OBJCOPY = avr-objcopy
DUDE = avrdude
SIZE = avr-size
MCU = atmega8

# If you are not using ATtiny85 and the USBtiny programmer, 
# update the lines below to match your configuration
CFLAGS = -Wall -Os -mmcu=$(MCU) -DF_CPU=8000000
OBJFLAGS = -j .text -j .data -O ihex
DUDEFLAGS = -p $(MCU) -c usbtiny -v
SIZEFLAGS = --mcu=$(MCU) -C 

# Object files 
OBJECTS = main.o

# By default, build the firmware and command-line client, but do not flash
all: main.hex

# With this, you can flash the firmware by just typing "make flash" on command-line
flash: main.hex
	$(DUDE) $(DUDEFLAGS) -U flash:w:$<

eeprom: main.eep
	$(DUDE) $(DUDEFLAGS) -U eeprom:w:$<

# Housekeeping if you want it
clean:
	$(RM) *.o *.hex *.elf

# From .elf file to .hex
%.hex: %.elf
	$(OBJCOPY) $(OBJFLAGS) $< $@
	
# outputs the size requirements on the microcontroller
size: main.hex
	$(SIZE) $(SIZEFLAGS) main.elf

# Main.elf requires additional objects to the firmware, not just main.o
main.elf: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@

# From C source to .o object file
%.o: %.c	
	$(CC) $(CFLAGS) -c $< -o $@

# From assembler source to .o object file
%.o: %.S
	$(CC) $(CFLAGS) -x assembler-with-cpp -c $< -o $@
