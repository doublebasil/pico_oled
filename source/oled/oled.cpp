#include "oled.hpp"

#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/spi.h"

static inline void m_displayInit( void );
static inline void m_chipSelect( void );
static inline void m_chipDeselect( void );
static inline void m_writeReg( uint8_t reg );
static inline void m_writeData( uint8_t data );

// IF THIS ISN'T WORKING IT MIGHT BE BECAUSE OF m_spiInstance

static int8_t m_csPin;
static int8_t m_dcPin;
static int8_t m_rstPin;
static int8_t m_spiInstance;
static uint8_t m_displayWidth;
static uint8_t m_displayHeight;

// Initialise GPIO and SPI
int oled_init( int8_t dinPin, int8_t clkPin, int8_t csPin, int8_t dcPin, 
    int8_t rstPin, int8_t spiOutput, unsigned int baudrate, uint8_t displayWidth,
    uint8_t displayHeight )
{
    // Set module variables
    m_csPin = csPin;
    m_dcPin = dcPin;
    m_rstPin = rstPin;
    m_displayWidth = displayWidth;
    m_displayHeight = displayHeight;

    // Set cs, dc and rst pins to be GPIO output pins
    gpio_init( m_csPin );
    gpio_init( m_dcPin );
    gpio_init( m_rstPin );
    gpio_set_dir( m_csPin, GPIO_OUT );
    gpio_set_dir( m_dcPin, GPIO_OUT );
    gpio_set_dir( m_rstPin, GPIO_OUT );
    // Set cs, dc and rst pins to high output
    gpio_put( m_csPin, 1 );
    gpio_put( m_dcPin, 1 );
    gpio_put( m_rstPin, 1 );

    // Set the SPI pins
    gpio_set_function( clkPin, GPIO_FUNC_SPI );
    gpio_set_function( dinPin, GPIO_FUNC_SPI );

    // Check din and clk pins are suitable, and init SPI at specified baud rate
    if( ( spiOutput == 0 ) &&
        ( ( dinPin == 3 ) || ( dinPin == 7 ) || ( dinPin == 19 ) ) && // Check din pin is connected to a SPI0 TX
        ( ( clkPin == 2 ) || ( clkPin == 6 ) || ( clkPin == 18 ) ) )  // Check clk pin is connected to a SPI0 SCK
    {
        spi_init( spi0, baudrate );
        m_spiInstance = 0;
        /* Set the SPI format to be the same as Arduino's SPI Mode 3, i.e.:
         * Most significant bit first, clock polarity = 1, clock phase = 1
         * This line is not needed, but it's good to ensure the correct settings */
        spi_set_format( spi0, 8U, SPI_CPOL_1, SPI_CPHA_1, SPI_MSB_FIRST );
    }
    else if( ( spiOutput == 1 ) &&
        ( ( dinPin == 11 ) || ( dinPin == 15 ) ) && // Check din pin is connected to a SPI1 TX
        ( ( clkPin == 10 ) || ( clkPin == 14 ) ) )  // Check clk pin is connected to a SPI1 SCK
    {
        spi_init( spi1, baudrate );
        // m_spiInstance is so that 
        m_spiInstance = 1;
        /* Set the SPI format to be the same as Arduino's SPI Mode 3, i.e.:
         * Most significant bit first, clock polarity = 1, clock phase = 1
         * This line is not needed, but it's good to ensure the correct settings */
        spi_set_format( spi1, 8U, SPI_CPOL_1, SPI_CPHA_1, SPI_MSB_FIRST );
    }
    else
    {
        // Invalid settings
        return 1;
    }

    gpio_put( m_rstPin, 1 );
    sleep_ms( 100U );
    gpio_put( m_rstPin, 0 );
    sleep_ms( 100U );
    gpio_put( m_rstPin, 1 );

    m_displayInit();

    sleep_ms( 200U );

    // Turn on the display
    m_writeReg(0xAF);

    sleep_ms( 500U );

    oled_clear();

    return 0;
}

void oled_clear( void )
{
    // I wrote this comment:
    // "This code cannot be used for setting the display to any other color"
    // But not sure I believe my past self

    m_writeReg(0x15);
    m_writeData(0);
    m_writeData(127);
    m_writeReg(0x75);
    m_writeData(0);
    m_writeData(127);

    m_writeReg(0x5C);

    for( uint16_t i = 0; i < m_displayWidth * m_displayHeight * 2U; ++i ) {
        m_writeData(0x0000);
    }
}

void oled_test( void )
{
    uint16_t color;
    uint8_t drawMode = 0;
    for( ;; )
    {
        for( uint8_t y = 0U; y < m_displayHeight; ++y )
        {
            for( uint8_t x = 0U; x < m_displayWidth; ++x )
            {
                if( drawMode == 0 )
                    color = ( (uint16_t) x * (uint16_t) x) + ( (uint16_t) y * (uint16_t) y);
                else if( drawMode == 1 )
                    color = ( (uint16_t) x * (uint16_t) x) - ( (uint16_t) y * (uint16_t) y);

                m_writeReg(0x15);
                m_writeData(x);
                m_writeData(x);
                m_writeReg(0x75);
                m_writeData(y);
                m_writeData(y);
                // fill!
                m_writeReg(0x5C);   
                
                // The 16 bits are sent in two seperate bytes
                m_writeData(color >> 8);
                m_writeData(color);
            }
        }
        ++drawMode;
        if( drawMode == 2 )
            drawMode = 0;
    }
    
}

static inline void m_displayInit( void )
{
    // Now for some inexplicable magic from the manufacturer. Comments here are from the manufacturer
    m_writeReg(0xFD);  // Command lock
    m_writeData(0x12);
    m_writeReg(0xFD);  // Command lock
    m_writeData(0xB1);

    m_writeReg(0xAE);  // Display off
    m_writeReg(0xA4);  // Normal Display mode

    m_writeReg(0x15);  // Set column address
    m_writeData(0x00); // Column address start 00
    m_writeData(0x7F); // Column address end 127
    m_writeReg(0x75);  // Set row address
    m_writeData(0x00); // Row address start 00
    m_writeData(0x7F); // Row address end 127    

    m_writeReg(0xB3);
    m_writeData(0xF1);

    m_writeReg(0xCA);  
    m_writeData(0x7F);

    m_writeReg(0xA0);  // Set re-map & data format
    m_writeData(0x74); // Horizontal address increment

    m_writeReg(0xA1);  // Set display start line
    m_writeData(0x00); // Start 00 line

    m_writeReg(0xA2);  // Set display offset
    m_writeData(0x00);

    m_writeReg(0xAB);  
    m_writeReg(0x01);  

    m_writeReg(0xB4);  
    m_writeData(0xA0);   
    m_writeData(0xB5);  
    m_writeData(0x55);    

    m_writeReg(0xC1);  
    m_writeData(0xC8); 
    m_writeData(0x80);
    m_writeData(0xC0);

    m_writeReg(0xC7);  
    m_writeData(0x0F);

    m_writeReg(0xB1);  
    m_writeData(0x32);

    m_writeReg(0xB2);  
    m_writeData(0xA4);
    m_writeData(0x00);
    m_writeData(0x00);

    m_writeReg(0xBB);  
    m_writeData(0x17);

    m_writeReg(0xB6);
    m_writeData(0x01);

    m_writeReg(0xBE);
    m_writeData(0x05);

    m_writeReg(0xA6);
}

static inline void m_chipSelect( void )
{
    gpio_put( PICO_DEFAULT_SPI_CSN_PIN, 0 );
}

static inline void m_chipDeselect( void )
{
    gpio_put( PICO_DEFAULT_SPI_CSN_PIN, 1 );
}

static inline void m_writeReg( uint8_t reg )
{
    gpio_put( m_dcPin, 0 );
    m_chipSelect();
    if( m_spiInstance == 0 )
        spi_write_blocking( spi0, &reg, 1 );
    else
        spi_write_blocking( spi1, &reg, 1 );
    m_chipDeselect(); // Repeatedly selecting and deselecting is bad
}

static inline void m_writeData( uint8_t data )
{
    gpio_put( m_dcPin, 1 );
    m_chipSelect();
    if( m_spiInstance == 0 )
        spi_write_blocking( spi0, &data, 1 );
    else
        spi_write_blocking( spi1, &data, 1 );
    m_chipDeselect(); // Repeatedly selecting and deselecting is bad
}
