#ifndef OLED_HPP
#define OLED_HPP

#include <stdint.h>

/*
 * Function: oled_init
 * --------------------
 * Initialises GPIO and SPI to use the OLED display
 *
 * dinPin: GPIO on pico which is connected to din on display. Will be used for SPI
 * clkPin: GPIO on pico connected to clk (clock) on the display. Will be used for SPI
 * csPin: GPIO on pico connected to CS (Chip Select) on the display. Will be used as GPIO
 * dcPin: GPIO on pico connected to DC (Data/Command) on the display. Will be used as GPIO
 * rstPin: GPIO on pico connected to RST (Reset) on the display, Will be used as GPIO
 * spiOutput: set to 0 for pico SPI0 and 1 for pico SPI1. See pico pin diagram, depends on your din and clk
 *
 * returns: void
 */
int oled_init( int8_t dinPin, int8_t clkPin, int8_t csPin, int8_t dcPin, 
    int8_t rstPin, int8_t spiOutput, unsigned int baudrate );

/*
Fill the screen with something to test the display
*/
void oled_test( void );

#endif /* OLED_HPP */


