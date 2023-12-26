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

    int result = oled_loadingCircleInit( 64U, 64U, 60U, 20U, 0b0000000000011111, 0 );
    if( result != 0 )
    {
        printf("Circle thing failed with code %d\n", result);
        for(;;) {sleep_ms(1000);}
    }
    else
    {
        printf("Init successful\n");
    }
    // oled_fill( 0, 60+20, 127, 60+20, 0xFFFFU ); // y = 55
    // oled_fill( 0, 60-20, 127, 60-20, 0xFFFFU ); // y = 65
    // oled_fill( 60+20, 0U, 60+20, 127U, 0xFFFFU ); // x = 55
    // oled_fill( 60-20, 0U, 60-20, 127U, 0xFFFFU ); // x = 65
    // oled_loadingCircleDisplay( 252U );
    // // oled_setPixel(60, 60, 0xFFFFU);
    // sleep_ms( 500 );
    // oled_loadingCircleDisplay( 252U );
    // sleep_ms( 500 );
    // oled_loadingCircleDisplay( 0U );
    // sleep_ms( 500 );
    // oled_loadingCircleDisplay( 252U );

    // oled_loadingCircleDisplay( ( 63U * 3U ) + 20U );

    // oled_loadingCircleDisplay( 252U );

    bool state = true;
    uint8_t p = 1U;
    while( p != 0U )
    {
        // oled_setPixel( 60, 60, 0xFFFFU );
        if( state )
        {
            ++p;
            if( p == 252U )
            {
                state = false;
            }
        }
        else
        {
            --p;
        }
        printf("p=%d\n", p);
        oled_loadingCircleDisplay( p );
        if( p == 252U )
            sleep_ms( 500U );
        else
            sleep_ms( 5U );
    }

    // typedef enum
    // {
    //     increasing,
    //     decreasing,
    // } t_direction;
    // t_direction dir = increasing;
    // uint8_t diff = 25;
    // uint8_t p = (63U*3) - diff;
    // while( true )
    // {
    //     oled_loadingCircleDisplay( p );
    //     printf("p=%d\n", p);
    //     sleep_ms( 50 );

    //     if( dir == increasing )
    //     {
    //         ++p;
    //         if( p == ( (63U*3) + diff ) )
    //             dir = decreasing;
    //     }
    //     else
    //     {
    //         --p;
    //         if( p == ( (63U*3) - diff ) )
    //             dir = increasing;
    //     }
    // }

    printf( "End\n" );
    for( ;; ) 
    {
        sleep_ms( 1000U );
    }
}
