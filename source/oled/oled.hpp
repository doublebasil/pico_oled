#ifndef OLED_HPP
#define OLED_HPP

/* Settings for this module ------------------------------------------------ */
#define OLED_INCLUDE_TEST_FUNCTION
#define OLED_INCLUDE_LOADING_BAR_HORIZONTAL
#define OLED_INCLUDE_LOADING_BAR_ROUND
#define OLED_INCLUDE_FONT8                      // Uses ~760 bytes
#define OLED_INCLUDE_FONT12                     // Uses ~1144 bytes
#define OLED_INCLUDE_FONT16                     // Uses ~3044 bytes
#define OLED_INCLUDE_FONT20                     // Uses ~3804 bytes
#define OLED_INCLUDE_FONT24                     // Uses ~6844 bytes
#define OLED_WRITE_TEXT_CHARACTER_GAP     ( 0 ) // Number of pixels between character

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
    int8_t rstPin, int8_t spiOutput, unsigned int baudrate, uint8_t displayWidth,
    uint8_t displayHeight );


/*
 * Function: oled_clear
 * --------------------
 * Set the display to black
 *
 * parameters: none
 *
 * returns: void
 */
void oled_clear( void );

/*
 * Function: oled_setPixel
 * --------------------
 * Set the colour of a specific pixel
 *
 * x: x coordinate of pixel
 * y: y coordinate of pixel
 * colour: Pixel colour in RGB565 format
 *
 * returns: void
 */
void oled_setPixel( uint8_t x, uint8_t y, uint16_t colour );

/*
 * Function: oled_fill
 * --------------------
 * Set the colour of a specific pixel
 *
 * x1: x coordinate of corner 1
 * y1: y coordinate of corner 1
 * x2: x coordinate of corner 2
 * y2: y coordinate of corner 2
 * colour: Pixel colour in RGB565 format
 *
 * returns: void
 */
void oled_fill( uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint16_t colour );

#ifdef OLED_INCLUDE_LOADING_BAR_HORIZONTAL
/*
 * Function: oled_loadingBarHorizontal
 * --------------------
 * Display a horizontal loading bar, specifying coordinates for the corners of
 * the bar. Progress measured in permille (out of 1000)
 *
 * barX1: X co-ordinate 1 of progress bar
 * barY1: Y co-ordinate 1 of progress bar
 * barX2: X co-ordinate 2 of progress bar
 * barY2: Y co-ordinate 2 of progress bar
 * permille: Progress out of 1000 (1000 would be done, 0 would be empty)
 * colour: Progress bar colour in RGB565
 * hasBorder: Set to true to draw a border around the progress bar
 *
 * returns: void
 */
void oled_loadingBarHorizontal( uint8_t barX1, uint8_t barY1, uint8_t barX2, 
    uint8_t barY2, uint16_t permille, uint16_t colour, bool hasBorder );
#endif /* OLED_INCLUDE_LOADING_BAR_HORIZONTAL */

#ifdef OLED_INCLUDE_TEST_FUNCTION
/*
 * Function: oled_test
 * --------------------
 * Removes everything on the screen, setting every pixel to black
 *
 * parameters: none
 *
 * returns: void
 */
void oled_test( void );
#endif /* OLED_INCLUDE_TEST_FUNCTION */

#if defined OLED_INCLUDE_FONT8 || defined OLED_INCLUDE_FONT12 || defined OLED_INCLUDE_FONT16 || defined OLED_INCLUDE_FONT20 || defined OLED_INCLUDE_FONT24
#define OLED_FONT8_WIDTH                ( 5 )
#define OLED_FONT12_WIDTH               ( 7 )
#define OLED_FONT16_WIDTH               ( 11 )
#define OLED_FONT20_WIDTH               ( 14 )
#define OLED_FONT24_WIDTH               ( 17 )

/*
 * Function: oled_writeChar
 * --------------------
 * Print a character to the display
 *
 * x: x coordinate of the top left position for the glyph
 * y: y coordinate of the top left position for the glyph
 * asciiCharacter: Character to be printed at specified position
 * fontSize: Height of character in pixels, can be 8, 12, 16, 20 or 24
 * colour: Character colour in RGB565 format
 *
 * returns: void
 */
void oled_writeChar( uint8_t x, uint8_t y, char asciiCharacter, uint8_t fontSize, uint16_t colour );

/*
 * Function: oled_writeText
 * --------------------
 * Print a character array to the display.
 *
 * xStartPos: Display x coordinate of the top left of the first character to be written
 * yStartPos: Display y coordinate of the top left of the first character to be written
 * text: Text to write to display
 * fontSize: Height of text in pixels, can be 8, 12, 16, 20 or 24
 * colour: Text colour in RGB565 format
 * useTextWrapping: If true, when the end of the display is reached a new line will be started
 *
 * returns: void
 */
void oled_writeText( uint8_t xStartPos, uint8_t yStartPos, const char text[],
    uint8_t fontSize, uint16_t colour, bool useTextWrapping );

#endif /* defined OLED_INCLUDE_FONT8 || defined OLED_INCLUDE_FONT12 || defined OLED_INCLUDE_FONT16 || defined OLED_INCLUDE_FONT20 || defined OLED_INCLUDE_FONT24 */

#endif /* OLED_HPP */
