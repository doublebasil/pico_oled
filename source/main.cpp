#include <stdio.h>
#include <stdint.h>

#include "pico/stdlib.h"

#include "oled.hpp"

int main( void )
{
    // Initialise the debug output
    stdio_init_all();
    // Wait for the stdio to initialise
    sleep_ms( 800U );

    /* PICO               | Display
     * -----------------------------
     * GPIO 19 (SPI0 TX)  | DIN
     * GPIO 18 (SPI0 SCK) | CLK
     * GPIO 17            | CS
     * GPIO 16            | DC
     * GPIO 20            | RST
     */
	if( oled_init( 19, 18, 17, 16, 20, 0, 14000000U ) != 0 )
    {
        printf( "oled_init failed\n" );
        return 1;
    }

    oled_test();

    for( ;; ) 
    {
        printf( "Hello world\n" );
        sleep_ms( 1000 );
    }
}
