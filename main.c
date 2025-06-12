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


// Main Loop
int main(void) {
  initSPI();
  initMatrix();
  clearMatrix();
  initBuffer();

  return 0;
}

