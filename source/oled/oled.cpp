#include "oled.hpp"

#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/spi.h"

static void inline m_chipSelect( void );
static void inline m_chipDeselect( void );

static int8_t m_csPin;
static int8_t m_dcPin;
static int8_t m_rstPin;

// Initialise GPIO and SPI
int oled_init( int8_t dinPin, int8_t clkPin, int8_t csPin, int8_t dcPin, 
    int8_t rstPin, int8_t spiOutput, unsigned int baudrate )
{
    m_csPin = csPin;
    m_dcPin = dcPin;
    m_rstPin = rstPin;

    // Set cs, dc and rst pins to be normal GPIO pins
    gpio_init( m_csPin );
    gpio_init( m_dcPin );
    gpio_init( m_rstPin );
    // Set cs, dc and rst pins to low output
    gpio_put( m_csPin, 0 );
    gpio_put( m_dcPin, 0 );
    gpio_put( m_rstPin, 0 );

    // Check spiOutput is 0 or 1
    if( ( spiOutput != 0 ) && ( spiOutput != 1 ) )
    {
        return 1;
    }

    // Init SPI at baud rate of 14MHz
    spi_init( spi0, 14000000 );

    // Set the SPI pins
    gpio_set_function( PICO_DEFAULT_SPI_SCK_PIN, GPIO_FUNC_SPI );
    gpio_set_function( PICO_DEFAULT_SPI_TX_PIN, GPIO_FUNC_SPI );

    return 0;
}

void oled_test( void )
{
    volatile int a = 0;
    ++a;
    // Not implemented
}

static void inline m_chipSelect( void ) {}
static void inline m_chipDeselect( void ) {}
