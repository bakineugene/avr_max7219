avr-gcc main.c -o main.elf -mmcu=atmega328p -DF_CPU=16000000UL -Os
avr-objcopy main.elf -O ihex main.hex
avrdude -c usbasp -p m328p -U flash:w:"main.hex":a
