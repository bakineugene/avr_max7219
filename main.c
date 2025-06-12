// Source - https://embeddedthoughts.com/2016/04/19/scrolling-text-on-the-8x8-led-matrix-with-max7219-drivers/

#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

// Macros to set SS line LOW (selected) or HIGH (deselected)
#define SLAVE_SELECT    PORTB &= ~(1 << PB2)
#define SLAVE_DESELECT  PORTB |= (1 << PB2)

#define NUM_DEVICES 2

// Initializes SPI
void initSPI(void) 
{
  DDRB  |= (1 << PB2);	    // Set SS output 
  PORTB |= (1 << PB2);      // Begin high (unselected)

  DDRB |= (1 << PB3);       // Output on MOSI 
  DDRB |= (1 << PB5);       // Output on SCK 

  SPCR |= (1 << MSTR);      // Clockmaster 
  SPCR |= (1 << SPE);       // Enable SPI
}

// Send byte through SPI
void writeByte(uint8_t byte)
{
  SPDR = byte;                      // SPI starts sending immediately  
  while(!(SPSR & (1 << SPIF)));     // Loop until complete bit set
}

// Sends word through SPI
void writeWord(uint8_t address, uint8_t data) 
{
  writeByte(address);	// Write first byte
  writeByte(data);      // Write Second byte
}

// Initializes all cascaded devices
void initMatrix() 
{
	uint8_t i;	// Var used in for loops

	// Set display brighness
	SLAVE_SELECT;
	for(i = 0; i < NUM_DEVICES; i++)   // Loop through number of cascaded devices
	{
		writeByte(0x0A); // Select Intensity register
		writeByte(0x07); // Set brightness
	}
	SLAVE_DESELECT;

	
	// Set display refresh
	SLAVE_SELECT;
	for(i = 0; i < NUM_DEVICES; i++)
	{
		writeByte(0x0B); // Select Scan-Limit register
		writeByte(0x07); // Select columns 0-7
	}
	SLAVE_DESELECT;

	 
	// Turn on the display
	SLAVE_SELECT;
	for(i = 0; i < NUM_DEVICES; i++)
	{
		writeByte(0x0C); // Select Shutdown register
		writeByte(0x01); // Select Normal Operation mode
	}
	SLAVE_DESELECT;

	 
	// Disable Display-Test
	SLAVE_SELECT;
	for(i = 0; i < NUM_DEVICES; i++)
	{
		writeByte(0x0F); // Select Display-Test register
		writeByte(0x00); // Disable Display-Test
	}
	SLAVE_DESELECT;
}

// Clears all columns on all devices
void clearMatrix(void)
{
	for(uint8_t x = 1; x < 9; x++) // for all columns
	{   
        SLAVE_SELECT;
        for(uint8_t i = 0; i < NUM_DEVICES; i++)
		{
			writeByte(x);    // Select column x
			writeByte(0x00); // Set column to 0
		}
		SLAVE_DESELECT;
	}
}

// Buffer array of bytes to store current display data for each column in each cascaded device
uint8_t buffer [NUM_DEVICES*8];	

// Initializes buffer empty
void initBuffer(void)
{   
	for(uint8_t i = 0; i < NUM_DEVICES*8; i++)
		buffer[i] = 0x00;
}       

// Displays current buffer on the cascaded devices
void displayBuffer()
{   
   for(uint8_t i = 0; i < NUM_DEVICES; i++) // For each cascaded device
   {
	   for(uint8_t j = 1; j < 9; j++) // For each column
	   {
		   SLAVE_SELECT;
		   
		   for(uint8_t k = 0; k < i; k++) // Write Pre No-Op code
			   writeWord(0x00, 0x00);
		   
		   writeWord(j, buffer[j + i*8 - 1]); // Write column data from buffer
		   
		   for(uint8_t k = NUM_DEVICES-1; k > i; k--) // Write Post No-Op code
			   writeWord(0x00, 0x00);
		   
		   SLAVE_DESELECT;
	   }
   }
}


// Main Loop
int main(void) {
    initSPI();
    initMatrix();
    clearMatrix();
    initBuffer();

	buffer[NUM_DEVICES*8 - 1] = 0b00111100;
	buffer[NUM_DEVICES*8 - 2] = 0b01001010;
	buffer[NUM_DEVICES*8 - 3] = 0b01001001;
	buffer[NUM_DEVICES*8 - 4] = 0b01001001;
	buffer[NUM_DEVICES*8 - 5] = 0b00110000;

	buffer[NUM_DEVICES*8 - 9] = 0b00111100;
	buffer[NUM_DEVICES*8 - 10] = 0b01001010;
	buffer[NUM_DEVICES*8 - 11] = 0b01001001;
	buffer[NUM_DEVICES*8 - 12] = 0b01001001;
	buffer[NUM_DEVICES*8 - 13] = 0b00110000;
    displayBuffer();	

    return 0;
}

