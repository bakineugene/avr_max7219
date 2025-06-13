#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

#define START_DATA_TRANSACTION()    PORTB &= ~(1 << PB2)
#define END_DATA_TRANSACTION()  PORTB |= (1 << PB2)

#define NUM_DEVICES 6

void initSPI(void) {
  DDRB |= (1 << PB2);	    // SS (CS - Chip Select)
  DDRB |= (1 << PB3);       // MOSI (DATA OUT)
  DDRB |= (1 << PB5);       // SCK (CLK)

  SPCR |= (1 << MSTR);      // Clockmaster 
  SPCR |= (1 << SPE);       // Enable SPI

  END_DATA_TRANSACTION();
}

void writeByte(uint8_t byte) {
  SPDR = byte;                      // SPI starts sending immediately  
  while(!(SPSR & (1 << SPIF)));     // Loop until complete bit set
}

void writeCommand(uint8_t command, uint8_t data) {
  writeByte(command);
  writeByte(data);
}

void initMatrix() {
	// Set display brighness
	START_DATA_TRANSACTION();
	for(int i = 0; i < NUM_DEVICES; i++) writeCommand(0x0A, 0x07);
	END_DATA_TRANSACTION();

	// Set display refresh
	START_DATA_TRANSACTION();
	for(int i = 0; i < NUM_DEVICES; i++) writeCommand(0x0B, 0x07);
	END_DATA_TRANSACTION();

	// Turn on the display
	START_DATA_TRANSACTION();
	for(int i = 0; i < NUM_DEVICES; i++) writeCommand(0x0C, 0x01);
	END_DATA_TRANSACTION();

	// Disable Display-Test
	START_DATA_TRANSACTION();
	for(int i = 0; i < NUM_DEVICES; i++) writeCommand(0x0F, 0x00);
	END_DATA_TRANSACTION();
}

uint8_t buffer [NUM_DEVICES * 8];	

void initBuffer() {
	for(int i = 0; i < NUM_DEVICES * 8; ++i) buffer[i] = 0x00;
}       

void sendBuffer() {   
   for(int column = 1; column < 9; ++column) {
       START_DATA_TRANSACTION();
       for(int i = 0; i < NUM_DEVICES; ++i) writeCommand(column, buffer[column + i*8 - 1]);
       END_DATA_TRANSACTION();
   }
}

int main(void) {
    initSPI();
    initMatrix();
    initBuffer();

    while(1) {
        for (int i = 0; i < 6; ++i) {
            buffer[NUM_DEVICES * 8 - i * 8 - 1] = 0b00011000;
            buffer[NUM_DEVICES * 8 - i * 8 - 2] = 0b00111100;
            buffer[NUM_DEVICES * 8 - i * 8 - 3] = 0b01111110;
            buffer[NUM_DEVICES * 8 - i * 8 - 4] = 0b11111111;
            buffer[NUM_DEVICES * 8 - i * 8 - 5] = 0b00011000;
            buffer[NUM_DEVICES * 8 - i * 8 - 6] = 0b00011000;
            buffer[NUM_DEVICES * 8 - i * 8 - 7] = 0b00011000;
            buffer[NUM_DEVICES * 8 - i * 8 - 8] = 0b00011000;
        }
        sendBuffer();	
        _delay_ms(500);

        for (int i = 0; i < 6; ++i) {
            buffer[NUM_DEVICES * 8 - i * 8 - 1] = 0b00001000;
            buffer[NUM_DEVICES * 8 - i * 8 - 2] = 0b00001100;
            buffer[NUM_DEVICES * 8 - i * 8 - 3] = 0b00001110;
            buffer[NUM_DEVICES * 8 - i * 8 - 4] = 0b11111111;
            buffer[NUM_DEVICES * 8 - i * 8 - 5] = 0b11111111;
            buffer[NUM_DEVICES * 8 - i * 8 - 6] = 0b00001110;
            buffer[NUM_DEVICES * 8 - i * 8 - 7] = 0b00001100;
            buffer[NUM_DEVICES * 8 - i * 8 - 8] = 0b00001000;
        }
        sendBuffer();
        _delay_ms(500);
	
        for (int i = 0; i < 6; ++i) {
            buffer[NUM_DEVICES * 8 - i * 8 - 1] = 0b00011000;
            buffer[NUM_DEVICES * 8 - i * 8 - 2] = 0b00011000;
            buffer[NUM_DEVICES * 8 - i * 8 - 3] = 0b00011000;
            buffer[NUM_DEVICES * 8 - i * 8 - 4] = 0b00011000;
            buffer[NUM_DEVICES * 8 - i * 8 - 5] = 0b11111111;
            buffer[NUM_DEVICES * 8 - i * 8 - 6] = 0b01111110;
            buffer[NUM_DEVICES * 8 - i * 8 - 7] = 0b00111100;
            buffer[NUM_DEVICES * 8 - i * 8 - 8] = 0b00011000;
        }
        sendBuffer();	
        _delay_ms(500);
	
        for (int i = 0; i < 6; ++i) {
            buffer[NUM_DEVICES * 8 - i * 8 - 1] = 0b00010000;
            buffer[NUM_DEVICES * 8 - i * 8 - 2] = 0b00110000;
            buffer[NUM_DEVICES * 8 - i * 8 - 3] = 0b01110000;
            buffer[NUM_DEVICES * 8 - i * 8 - 4] = 0b11111111;
            buffer[NUM_DEVICES * 8 - i * 8 - 5] = 0b11111111;
            buffer[NUM_DEVICES * 8 - i * 8 - 6] = 0b01110000;
            buffer[NUM_DEVICES * 8 - i * 8 - 7] = 0b00110000;
            buffer[NUM_DEVICES * 8 - i * 8 - 8] = 0b00010000;
        }
        sendBuffer();	
        _delay_ms(500);
    }
    return 0;
}

