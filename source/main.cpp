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

#define S sleep_ms( 500 );

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

    // oled_writeText( 10, 10, "hello", 12U, 0xFFFF, false );
    // sleep_ms( 1000ULL );
    // oled_clear();

    // for( uint16_t p = 0; p < 1000U; ++p )
    // {
    //     oled_loadingBarRound( 128U/2U, 128U/2U, 60U, 20U, p, 0b0000000000000011, true );
    //     sleep_ms( 100U );
    // }
    // // oled_fill( 0, 0, 128, 0, 0xFFFF );

    int result = oled_terminalInit( 16U, 0b0000000000011111U );
    if( result != 0 )
    {
        printf("terminal init failed with exit code %d\n", result);
        for( ;; )
            sleep_ms( 1000U );
    }

    int height = (int) oled_terminalGetHeightInCharacters();
    char txt[20];

    oled_terminalWrite("Test 1");
    S
    oled_terminalSetLine( 3 );
    S
    oled_terminalWriteTemp("Test 2");
    S
    oled_terminalWrite("Test 3");
    S
    oled_terminalSetLine( 0 );
    S

    for( int i = 0; i < height; i++ )
    {
        sprintf(txt, "%ld", (uint32_t) micros());
        oled_terminalWrite(txt);
        S
    }

    printf( "End\n" );
    for( ;; ) 
    {
        sleep_ms( 1000U );
    }
}
