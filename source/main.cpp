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

    char txt[20];
    for( int i = 0; i < 9; i++ )
    {
        uint32_t t = (uint32_t) millis();
        sprintf(txt, "T %ld", t);
        oled_terminalWriteTemp(txt);
        sleep_ms(500);
        sprintf(txt, "P %ld", t);
        oled_terminalWrite(txt);
        sleep_ms(200);
    }
    oled_terminalWriteTemp(".");
    sleep_ms(300);
    oled_terminalWriteTemp("..");
    sleep_ms(300);
    oled_terminalWriteTemp("...");
    sleep_ms(500);
    oled_terminalWrite("Done!");


    // uint16_t delay = 50;
    // oled_terminalWrite("a   ");
    // sleep_ms( delay );
    // oled_terminalWrite(" b  ");
    // sleep_ms( delay );
    // oled_terminalWrite("  c ");
    // sleep_ms( delay );
    // oled_terminalWriteTemp("mnop");
    // sleep_ms( 1000 );
    // oled_terminalWrite("abcdefghijklmnop");
    // sleep_ms( 1000 );
    // oled_terminalWrite("uvwx");
    // sleep_ms( 500 );
    // oled_terminalWrite("yz");
    // sleep_ms( 500 );
    // oled_terminalWrite("abcd");
    // oled_terminalWrite("abcd");
    
    // // oled_terminalWrite( "B" );
    // // sleep_ms( 500 );
    // // oled_terminalWrite( "Hello," );
    // // sleep_ms( 200 );
    // // oled_terminalWrite( "world!" );
    // // sleep_ms( 500 );

    // oled_terminalDeinit();
    // sleep_ms( 1000 );
    // oled_clear();
    // oled_terminalWrite( "BAD" );
    // sleep_ms( 500 );
    // oled_terminalInit( 8U, 0b1111100000000000 );

    // char str[10] = {0};
    // for( ;; )
    // {
    //     sprintf( str, "%ld", (uint32_t) micros() );
    //     oled_terminalWrite( str );
    //     sleep_ms( 50 );
    // }

    // oled_setPixel(0, 127, 0xFFF0);

    // oled_setPixel(0, 0, 0b1111100000000000);
    // oled_writeChar(0, 0, 'B', 24U, 0xFFFF );

    printf( "End\n" );
    for( ;; ) 
    {
        sleep_ms( 1000U );
    }
}
