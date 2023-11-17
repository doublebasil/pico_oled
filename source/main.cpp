#include <stdio.h>

#include "pico/stdlib.h"

#include "oled.h"

int main( void )
{
    // Initialise the debug output
    stdio_init_all();
    // Wait for the stdio to initialise
    sleep_ms( 800U );

	printf( "Initialised\n" );

    for( ;; ) 
    {
        printf( "Hello world\n" );
        sleep_ms( 1000 );
    }
}

