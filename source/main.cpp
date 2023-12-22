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

    int result = oled_loadingCircleInit( 4U, 4U, 5U, 0U, 0b1111100000000000U, 0 );
    if( result != 0 )
    {
        printf("Circle thing failed with code %d\n", result);
        for(;;) {sleep_ms(1000);}
    }
    else
    {
        printf("Init successfullll\n");
    }

    oled_loadingCircleDisplay( 100U );

    oled_setPixel( 0, 0, 0xFFFFU );

    // sleep_ms(1000);
    // oled_loadingCircleDeinit();
    // sleep_ms(500);
    // oled_clear();



    // oled_fill(0, 0, 127, 127, 0b1111100000000000U);
    // int result = oled_loadingBarInit( 0, 127, 1, 10, 0b0000000000011111U, 0 );
    // printf("loading bar init with %d\n", result);
    // sleep_ms(500);
    // while( result != 0 ) { sleep_ms(1000); }
    // for( uint16_t progress = 0; progress < 256; progress++ )
    // {
    //     printf("%d\n", progress);
    //     oled_loadingBarDisplay( progress );
    //     sleep_ms( 5 );
    // }
    // oled_loadingBarDeinit();

    // oled_loadingBarInit( 0, 127, 0, 127, 0xFFFF, 0 );
    // oled_loadingBarDisplay( 100 );
    // sleep_ms(500);
    // oled_loadingBarDisplay( 255U );
    // oled_loadingBarDeinit();

    // sleep_ms(500);
    // oled_clear();

    // oled_writeText( 10, 10, "hello", 12U, 0xFFFF, false );
    // sleep_ms( 1000ULL );
    // oled_clear();

    // for( uint16_t p = 0; p < 1000U; ++p )
    // {
    //     oled_loadingBarRound( 128U/2U, 128U/2U, 60U, 20U, p, 0b0000000000000011, true );
    //     sleep_ms( 100U );
    // }
    // oled_fill( 0, 0, 128, 0, 0xFFFF );

    // int result = oled_terminalInit( 16U, 0b0000011111100000U );
    // if( result != 0 )
    // {
    //     printf("terminal init failed with exit code %d\n", result);
    //     for( ;; )
    //         sleep_ms( 1000U );
    // }

    // int height = (int) oled_terminalGetHeightInCharacters();
    // char txt[20];

    // oled_terminalSetLine( 5 );
    // oled_terminalWrite("Test 1");
    // S
    // oled_terminalSetLine( 3 );
    // S
    // oled_terminalWriteTemp("Test 2");
    // S
    // oled_terminalWrite("Test 3");
    // S
    // oled_terminalSetLine( 0 );
    // S

    // for( int i = 0; i < height; i++ )
    // {
    //     sprintf(txt, "%ld", (uint32_t) micros());
    //     oled_terminalWrite(txt);
    //     S
    // }

    printf( "End\n" );
    for( ;; ) 
    {
        sleep_ms( 1000U );
    }
}
