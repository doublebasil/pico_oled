#include <stdio.h>
#include <stdint.h>
#include <string.h>

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
	if( oled_init( 19, 18, 17, 16, 20, 0, 14000000U, 128U, 128U ) != 0 )
    {
        printf( "oled_init failed\n" );
        return 1;
    }

    // oled_test();
    
    // for( char c = 32; c < 127; ++c )
    // {
    //     oled_clear();
    //     oled_writeChar( 10U, 10U, c, 16U, 0xFFFF );
    //     sleep_ms( 500U );
    // }

    // char text[] = "Hello,";
    // oled_writeText( 0U, 0U, &text[0], (uint8_t) sizeof(text) / sizeof(text[0]), 24, 0xC01FU, false );
    // char text2[] = "world!";
    // oled_writeText( 0U, 25U, &text2[0], (uint8_t) sizeof(text2) / sizeof(text2[0]), 24, 0xC01FU, false );

    // char text[] = "abcdefghijklmnopqrstuvwxyz";
    // oled_writeText( 0U, 0U, &text[0], (uint8_t) sizeof(text) / sizeof(text[0]), 24, 0b0000000000011111U, true );

    oled_writeText( 0U, 0U, "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789", 24, 0b0000000000011111U, true );

    for( ;; ) 
    {
        printf( "End\n" );
        sleep_ms( 1000U );
    }
}
