make: 
	
	avr-gcc -DF_CPU=13000000 -Os -mmcu=atmega328p main.c
	avr-objcopy -O binary a.out a.bin
	
	avrdude -U flash:w:a.bin -p m328p -c usbasp -p m328p