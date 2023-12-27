#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "pico/stdlib.h"
#include "pico/time.h"

#include "oled.hpp"

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

    oled_fill( 0, 0, 127, 127, 0b0000000000011111 );

    for( int i = 0; i < 10; i++ )
    {
        sleep_ms( 100 );
        printf("Booting\n");
    }

    // Initialise the SD card
    if( !sd_init_driver() )
    {
        printf( "Couldn't initialise the SD card :(\n" );
        while( true );
    }
    else
        printf( "SD card initialised ok!\n" );

    int result;

    result = oled_sdWriteImage( "example2.txt", 20, 40 );
    printf("sdWriteImage returned %d\n", result);

    sleep_ms( 1000 );

    result = oled_sdWriteImage( "example1.txt", 0, 0 );
    printf("sdWriteImage returned %d\n", result);


    // bool state = true;
    // uint8_t p = 1U;
    // while( p != 0U )
    // {
    //     // oled_setPixel( 60, 60, 0xFFFFU );
    //     if( state )
    //     {
    //         ++p;
    //         if( p == 252U )
    //         {
    //             state = false;
    //         }
    //     }
    //     else
    //     {
    //         --p;
    //     }
    //     printf("p=%d\n", p);
    //     oled_loadingCircleDisplay( p );
    //     if( p == 252U )
    //         sleep_ms( 500U );
    //     else
    //         sleep_ms( 5U );
    // }

    printf( "End\n" );
    for( ;; ) 
    {
        sleep_ms( 1000U );
    }
}
