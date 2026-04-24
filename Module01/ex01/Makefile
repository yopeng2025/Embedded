MCU = atmega328p
F_CPU = 16000000UL #16MHz (MillionHertz)

CC = avr-gcc
CFLAGS = -Wall -Wextra -Werror -Wundef -mmcu=$(MCU) -DF_CPU=$(F_CPU) -O2

OBJCOPY = avr-objcopy
AVRDUDE = avrdude
PORT = /dev/ttyUSB0

all: hex flash

hex:
	$(CC) $(CFLAGS) main.c -o main.bin
	$(OBJCOPY) -O ihex -R .eeprom main.bin main.hex

flash:
	$(AVRDUDE) -p $(MCU) -c arduino -P $(PORT) -U flash:w:main.hex:i

clean:
	rm -f main.hex main.bin main.s