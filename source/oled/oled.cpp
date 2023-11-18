#include "oled.hpp"

#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/spi.h"

static void inline m_chipSelect( void );
static void inline m_chipDeselect( void );
static inline void m_writeReg( uint8_t reg );
static inline void m_writeData( uint8_t data );

// IF THIS ISN'T WORKING IT MIGHT BE BECAUSE OF m_spiInstance

static int8_t m_csPin;
static int8_t m_dcPin;
static int8_t m_rstPin;
static int8_t m_spiInstance;

// Initialise GPIO and SPI
int oled_init( int8_t dinPin, int8_t clkPin, int8_t csPin, int8_t dcPin, 
    int8_t rstPin, int8_t spiOutput, unsigned int baudrate )
{
    // Set module variables
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

    // Check din and clk pins are suitable, and init SPI at specified baud rate
    if( ( spiOutput == 0 ) &&
        ( ( dinPin == 3 ) || ( dinPin == 7 ) || ( dinPin == 19 ) ) && // Check din pin is connected to a SPI0 TX
        ( ( clkPin == 2 ) || ( clkPin == 6 ) || ( clkPin == 18 ) ) )  // Check clk pin is connected to a SPI0 SCK
    {
        spi_init( spi0, baudrate );
        m_spiInstance = 0;
    }
    else if( ( spiOutput == 1 ) &&
        ( ( dinPin == 11 ) || ( dinPin == 15 ) ) && // Check din pin is connected to a SPI1 TX
        ( ( clkPin == 10 ) || ( clkPin == 14 ) ) )  // Check clk pin is connected to a SPI1 SCK
    {
        spi_init( spi1, baudrate );
        m_spiInstance = 1;
    }
    else
    {
        // Invalid settings
        return 1;
    }

    // Set the SPI pins
    gpio_set_function( clkPin, GPIO_FUNC_SPI );
    gpio_set_function( dinPin, GPIO_FUNC_SPI );

    gpio_put( m_rstPin, 1 );
    // questionable delay length incoming

    return 0;
}

void oled_test( void )
{
    // TODO
}

static void inline m_chipSelect( void )
{
    // Wait 3 cpu cycles (nop is a asm command which does nothing)
    asm volatile( "nop \n nop \n nop" ); // Definitely consider getting rid of this once things are working
    gpio_put( PICO_DEFAULT_SPI_CSN_PIN, 0 );
    asm volatile( "nop \n nop \n nop" );
}

static void inline m_chipDeselect( void )
{
    // Wait 3 cpu cycles (nop is a asm command which does nothing)
    asm volatile( "nop \n nop \n nop" ); // Definitely consider getting rid of this once things are working
    gpio_put( PICO_DEFAULT_SPI_CSN_PIN, 1 );
    asm volatile( "nop \n nop \n nop" );
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
// Shouldn't mix void inline with inline void