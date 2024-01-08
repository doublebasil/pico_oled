#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "pico/stdlib.h"
#include "pico/time.h"

#include "oled.hpp"

#include "sd_card.h"
#include "ff.h"

inline uint64_t micros()
{
    return to_us_since_boot( get_absolute_time() );
}
inline uint64_t millis()
{
    return micros() / 1000ULL;
}

int main( void )
{
    // Initialise the debug output
    stdio_init_all();
    // // Wait for the stdio to initialise
    // sleep_ms( 800U );

    /* PICO               | Display
     * ---------------------------------------
     * GPIO 19 (SPI0 TX)  | DIN
     * GPIO 18 (SPI0 SCK) | CLK
     * GPIO 17            | CS
     * GPIO 16            | DC
     * GPIO 20            | RST
     * 
     * PICO               | SD Card, if using
     * ---------------------------------------
     * 5V                 | VCC
     * Ground             | Ground
     * GPIO 8 (SPI1 RX)   | MISO
     * GPIO 11 (SPI1 TX)  | MOSI
     * GPIO 10 (SPI SCK)  | SCK
     * GPIO 9 (SPI1 CSn)  | CS
     */
	if( oled_init( 19, 18, 17, 16, 20, 0, 14000000U, 128U, 128U ) != 0 )
    {
        printf( "oled_init failed\n" );
        return 1;
    }

    for( int i = 0; i < 10; i++ )
    {
        sleep_ms( 100 );
        printf("Booting\n");
    }

    // oled_terminalInit( 24, 0b1111100000000000U );
    // oled_terminalWrite( "Hello" );
    // oled_terminalWriteTemp( "world" );
    // sleep_ms( 1000 );
    // oled_terminalSetNewColour( 0b0000000000011111U );
    // sleep_ms( 1000 );
    // oled_terminalWrite( "test" );

    oled_loadingCircleInit( 64, 64, 40, 20, 0b0000000000011111 );
    for( int i = 0; i <= 252; i++ )
    {
        oled_loadingCircleDisplay( i );
        sleep_ms( 10 );
    }

    // // Initialise the SD card
    // if( !sd_init_driver() )
    // {
    //     printf( "Couldn't initialise the SD card :(\n" );
    //     while( true );
    // }
    // else
    //     printf( "SD card initialised ok!\n" );

    // int result;
    // result = oled_sdWriteImage( "example2.txt", 20, 40 );
    // printf("sdWriteImage returned %d\n", result);

    // sleep_ms( 1000 );

    // result = oled_sdWriteImage( "example1.txt", 0, 0 );
    // printf("sdWriteImage returned %d\n", result);

    
    
    // oled_fill( 0, 0, 127, 127, 0xFFFFU );
    // int qrResult;
    // qrResult = oled_printQrCode( "http://192.168.1.120", 0x0000U, 0b0000000000011111U );
    // printf( "qrResult=%d\n", qrResult );

    printf( "End\n" );
    for( ;; ) 
    {
        sleep_ms( 1000U );
    }
}
