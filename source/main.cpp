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

    const uint32_t del = 750;
    oled_terminalInit( 24, 0b1111100000000000U );
    // oled_terminalWrite( "the" );
    // sleep_ms( del );
    // oled_terminalWrite( "quick" );
    // sleep_ms( del );
    // oled_terminalWrite( "brown" );
    // sleep_ms( del );
    // // oled_terminalSetHeight( 2 );
    // oled_terminalWrite( "fox" );
    // sleep_ms( del );
    // oled_terminalWrite( "jumps" );
    // sleep_ms( del );
    // // oled_terminalClear();
    // // sleep_ms( del );
    // oled_terminalWrite( "over" );
    // sleep_ms( del );
    // oled_terminalWrite( "the" );
    // sleep_ms( del );
    // oled_terminalWrite( "lazy" );
    // sleep_ms( del );
    // oled_terminalWrite( "dog" );
    

    char text[30];
    snprintf( text, sizeof( text ), "TermH=%d", oled_terminalGetHeightInCharacters() );
    oled_terminalWrite("test1");
    oled_terminalWrite(text);
    oled_terminalWrite("test3");
    sleep_ms( del );
    oled_terminalSetLine( oled_terminalGetHeightInCharacters() ); // Move to end of terminal
    oled_terminalWrite( "ABCDE" ); // Write something, the test1-3 will get scrolled up
    sleep_ms( del );
    oled_terminalSetLine( oled_terminalGetHeightInCharacters() ); // Ensure still at the end of the terminal
    oled_terminalWriteNoScroll( "HIJKL" ); // Write something, Other things will not be scrolled

    // oled_terminalWriteTemp( "world" );
    // sleep_ms( 1000 );
    // oled_terminalSetNewColour( 0b0000000000011111U );
    // sleep_ms( 1000 );
    // oled_terminalWrite( "test" );

    // oled_loadingCircleInit( 64, 64, 40, 20, 0b0000000000011111 );
    // for( int i = 0; i <= 252; i++ )
    // {
    //     oled_loadingCircleDisplay( i );
    //     sleep_ms( 10 );
    // }

    // oled_terminalInit( 12, 0xFFFF );
    // oled_terminalWriteTemp( "Hello" );
    // sleep_ms( 500 );
    // oled_terminalWriteTemp( "World" );
    // sleep_ms( 500 );
    // oled_terminalWriteTemp( "ABCDEFG" );

    // gpio_init( 2 );
    // gpio_set_dir( 2, GPIO_IN );
    // oled_terminalInit( 24, 0xFFFFU );
    

    // oled_drawLineBetweenPoints( 0, 0, 127, 127, 0b1111100000000000U, 0 );
    // oled_drawLineBetweenPoints( 10, 20, 20, 127, 0b0000000000011111U, 1 );
    // oled_drawLineBetweenPoints( 20, 63, 100, 70, 0b0000011111100000U, 2 );
    // oled_drawLineBetweenPoints( 80, 100, 110, 112, 0xFFFFU, 3 );

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
