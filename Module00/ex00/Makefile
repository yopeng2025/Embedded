# avr mcu（Microcontroller Unit）
MCU = atmega328p
# mcu clock frequency: 16MHz (Megahertz)(UL:Unsigned Long)
F_CPU = 16000000UL

CC = avr-gcc
# -mmcu: generate machine code specific to the AVR architecture（atmage328p in this case）
# -D: Define CPU frequency macro (to calculate the correct number of clock cycles based on the 16MHz clock)
# -O2: level 2 Optimizate code (-O0 not optimize -O1 light optimize -O2 standard balance)
CFLAGS = -Wall -Wextra -Werror -Wundef -mmcu=$(MCU) -DF_CPU=$(F_CPU) -O2
OBJCOPY = avr-objcopy
AVRDUDE = avrdude
# Hardware Connection Port
PORT = /dev/ttyUSB0

all: hex flash

# $(OBJCOPY):  extract data from .bin file (light version, cauz without .eeprom)
#  -O ihex: translate file into "Intel HEX(raw machine code & memory addresses required by microcontroller)"
#  -R .eeprom: remove .eeprom(memory for data) from hex file, only need Flash(memory for code) in hex file
# main.bin as INPUT & main.hex as OUTPUT
hex:
	$(CC) $(CFLAGS) main.c -o main.bin
	$(OBJCOPY) -O ihex -R .eeprom main.bin main.hex

# AVRDUDE: AVR Downloader Uploader
#   -p: Partnumber (check hardware compatibility)
#	-c: Config programmer(using arduino)
#	-P: Port
#	-U: Update instruction(flash:place to store memory; w:write; main.hex:source file to be uploaded; i:Intel Hex format)
flash: 
	$(AVRDUDE) -p $(MCU) -c arduino -P $(PORT) -U flash:w:main.hex:i

clean:
	rm -f main.hex main.bin
