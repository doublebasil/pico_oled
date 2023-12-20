#include "oled.hpp"

#include <stdio.h> // Just for debugging
#ifdef OLED_INCLUDE_LOADING_BAR_ROUND
#include <math.h>
#endif

#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/spi.h"

#ifdef OLED_INCLUDE_FONT8
#include "font8.h"
#endif /* OLED_INCLUDE_FONT8 */
#ifdef OLED_INCLUDE_FONT12
#include "font12.h"
#endif /* OLED_INCLUDE_FONT12 */
#ifdef OLED_INCLUDE_FONT16
#include "font16.h"
#endif /* OLED_INCLUDE_FONT16 */
#ifdef OLED_INCLUDE_FONT20
#include "font20.h"
#endif /* OLED_INCLUDE_FONT20 */
#ifdef OLED_INCLUDE_FONT24
#include "font24.h"
#endif /* OLED_INCLUDE_FONT24 */

static int8_t m_csPin;
static int8_t m_dcPin;
static int8_t m_rstPin;
static int8_t m_spiInstance;
static uint8_t m_displayWidth;
static uint8_t m_displayHeight;
#if defined OLED_INCLUDE_FONT8 || defined OLED_INCLUDE_FONT12 || defined OLED_INCLUDE_FONT16 || defined OLED_INCLUDE_FONT20 || defined OLED_INCLUDE_FONT24
static uint8_t* m_terminalBitmapPtr1 = NULL;
static uint8_t* m_terminalBitmapPtr2 = NULL;
typedef enum
{
    e_terminalUninitialised,
    e_terminalBitmap1Next,
    e_terminalBitmap2Next,
} t_terminalBitmapState;
static t_terminalBitmapState m_terminalBitmapState = e_terminalUninitialised;
static uint8_t m_terminalFontSize;
static uint16_t m_terminalFontColour;
static uint8_t m_terminalCurrentLine;
static tFontTable* m_terminalFontTablePtr;
#endif // defined OLED_INCLUDE_FONT8 || defined OLED_INCLUDE_FONT12 || defined OLED_INCLUDE_FONT16 || defined OLED_INCLUDE_FONT20 || defined OLED_INCLUDE_FONT24

static uint16_t TEMP_callocSize;

#if defined OLED_INCLUDE_FONT8 || defined OLED_INCLUDE_FONT12 || defined OLED_INCLUDE_FONT16 || defined OLED_INCLUDE_FONT20 || defined OLED_INCLUDE_FONT24
void m_terminalPushBitmap( void );
static inline void m_terminalWriteChar( char character, uint8_t textOriginX,
    uint8_t textOriginY );
#endif // defined OLED_INCLUDE_FONT8 || defined OLED_INCLUDE_FONT12 || defined OLED_INCLUDE_FONT16 || defined OLED_INCLUDE_FONT20 || defined OLED_INCLUDE_FONT24
#ifdef OLED_INCLUDE_LOADING_BAR_ROUND
static const int16_t cosLookupTable[91] = {
	1000, 1000, 999, 999, 
	998, 996, 995, 993, 
	990, 988, 985, 982, 
	978, 974, 970, 966, 
	961, 956, 951, 946, 
	940, 934, 927, 921, 
	914, 906, 899, 891, 
	883, 875, 866, 857, 
	848, 839, 829, 819, 
	809, 799, 788, 777, 
	766, 755, 743, 731, 
	719, 707, 695, 682, 
	669, 656, 643, 629, 
	616, 602, 588, 574, 
	559, 545, 530, 515, 
	500, 485, 469, 454, 
	438, 423, 407, 391, 
	375, 358, 342, 326, 
	309, 292, 276, 259, 
	242, 225, 208, 191, 
	174, 156, 139, 122, 
	105, 87, 70, 52, 
	35, 17, 0};
#endif /* OLED_INCLUDE_LOADING_BAR_ROUND */
static inline void m_displayInit( void );
static inline void m_chipSelect( void );
static inline void m_chipDeselect( void );
static inline void m_writeReg( uint8_t reg );
static inline void m_writeData( uint8_t data );
#ifdef OLED_INCLUDE_LOADING_BAR_ROUND
static inline int16_t m_intsin( int16_t angle );
static inline int16_t m_intcos( int16_t angle );
#endif /* OLED_INCLUDE_LOADING_BAR_ROUND */


// Initialise GPIO and SPI
int oled_init( int8_t dinPin, int8_t clkPin, int8_t csPin, int8_t dcPin, 
    int8_t rstPin, int8_t spiOutput, unsigned int baudrate, uint8_t displayWidth,
    uint8_t displayHeight )
{
    // Set module variables
    m_csPin = csPin;
    m_dcPin = dcPin;
    m_rstPin = rstPin;
    m_displayWidth = displayWidth;
    m_displayHeight = displayHeight;

    // Set cs, dc and rst pins to be GPIO output pins
    gpio_init( m_csPin );
    gpio_init( m_dcPin );
    gpio_init( m_rstPin );
    gpio_set_dir( m_csPin, GPIO_OUT );
    gpio_set_dir( m_dcPin, GPIO_OUT );
    gpio_set_dir( m_rstPin, GPIO_OUT );
    // Set cs, dc and rst pins to high output
    gpio_put( m_csPin, 1 );
    gpio_put( m_dcPin, 1 );
    gpio_put( m_rstPin, 1 );

    // Set the SPI pins
    gpio_set_function( clkPin, GPIO_FUNC_SPI );
    gpio_set_function( dinPin, GPIO_FUNC_SPI );

    // Check din and clk pins are suitable, and init SPI at specified baud rate
    if( ( spiOutput == 0 ) &&
        ( ( dinPin == 3 ) || ( dinPin == 7 ) || ( dinPin == 19 ) ) && // Check din pin is connected to a SPI0 TX
        ( ( clkPin == 2 ) || ( clkPin == 6 ) || ( clkPin == 18 ) ) )  // Check clk pin is connected to a SPI0 SCK
    {
        spi_init( spi0, baudrate );
        m_spiInstance = 0;
        /* Set the SPI format to be the same as Arduino's SPI Mode 3, i.e.:
         * Most significant bit first, clock polarity = 1, clock phase = 1
         * This line is not needed, but it's good to ensure the correct settings */
        spi_set_format( spi0, 8U, SPI_CPOL_1, SPI_CPHA_1, SPI_MSB_FIRST );
    }
    else if( ( spiOutput == 1 ) &&
        ( ( dinPin == 11 ) || ( dinPin == 15 ) ) && // Check din pin is connected to a SPI1 TX
        ( ( clkPin == 10 ) || ( clkPin == 14 ) ) )  // Check clk pin is connected to a SPI1 SCK
    {
        spi_init( spi1, baudrate );
        // m_spiInstance is so that 
        m_spiInstance = 1;
        /* Set the SPI format to be the same as Arduino's SPI Mode 3, i.e.:
         * Most significant bit first, clock polarity = 1, clock phase = 1
         * This line is not needed, but it's good to ensure the correct settings */
        spi_set_format( spi1, 8U, SPI_CPOL_1, SPI_CPHA_1, SPI_MSB_FIRST );
    }
    else
    {
        // Invalid settings
        return 1;
    }

    m_chipSelect();

    gpio_put( m_rstPin, 1 );
    sleep_ms( 10U );
    gpio_put( m_rstPin, 0 );
    sleep_ms( 10U );
    gpio_put( m_rstPin, 1 );

    m_displayInit();

    sleep_ms( 10U );

    // Turn on the display
    m_writeReg(0xAF);

    sleep_ms( 10U );

    oled_clear();
    // CS is now inactive (high)

    return 0;
}

void oled_clear( void )
{
    m_chipSelect();
    m_writeReg(0x15);
    m_writeData(0);
    m_writeData(127);
    m_writeReg(0x75);
    m_writeData(0);
    m_writeData(127);

    m_writeReg(0x5C);

    for( uint16_t i = 0; i < m_displayWidth * m_displayHeight; ++i )
    {
        m_writeData( 0x00 );
        m_writeData( 0x00 );
    }
    m_chipDeselect();
}

void oled_setPixel( uint8_t x, uint8_t y, uint16_t colour )
{
    if( ( x < 0U ) || ( y < 0U ) || ( x > m_displayWidth ) || ( y > m_displayHeight ) )
    {
        // Pixel is out of bounds
        return;
    }

    m_chipSelect();

    m_writeReg(0x15);
    m_writeData(x);
    m_writeData(x);
    m_writeReg(0x75);
    m_writeData(y);
    m_writeData(y);
    m_writeReg(0x5C);   
    
    // The 16 bits are sent in two bytes
    m_writeData(colour >> 8);
    m_writeData(colour);

    m_chipDeselect();
}

void oled_fill( uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint16_t colour )
{
    uint8_t xMin;
    uint8_t xMax;
    uint8_t yMin;
    uint8_t yMax;
    if( x1 < x2 )
    {
        xMin = x1;
        xMax = x2;
    }
    else
    {
        xMin = x2;
        xMax = x1;
    }
    if( y1 < y2 )
    {
        yMin = y1;
        yMax = y2;
    }
    else
    {
        yMin = y2;
        yMax = y1;
    }
    for( uint8_t x = xMin; x <= xMax; ++x )
    {
        for( uint8_t y = yMin; y <= yMax; ++y )
        {
            oled_setPixel( x, y, colour );
        }
    }
}

#ifdef OLED_INCLUDE_LOADING_BAR_HORIZONTAL
void oled_loadingBarHorizontal( uint8_t barX1, uint8_t barY1, uint8_t barX2, 
    uint8_t barY2, uint16_t permille, uint16_t colour, bool hasBorder )
{
    if( hasBorder == true )
    {
        oled_fill( barX1, barY1, barX1, barY2, colour );
        oled_fill( barX1, barY1, barX2, barY1, colour );
        oled_fill( barX1, barY2, barX2, barY2, colour );
        oled_fill( barX2, barY1, barX2, barY2, colour );
    }

    uint8_t xMin;
    uint8_t xMax;
    uint8_t yMin;
    uint8_t yMax;
    if( barX1 < barX2 )
    {
        xMin = barX1;
        xMax = barX2;
    }
    else
    {
        xMin = barX2;
        xMax = barX1;
    }
    if( barY1 < barY2 )
    {
        yMin = barY1;
        yMax = barY2;
    }
    else
    {
        yMin = barY2;
        yMax = barY1;
    }

    oled_fill( xMin, yMin, ( ( ( xMax - xMin ) * permille ) / 1000U ) + xMin, yMax, colour );
}
#endif /* OLED_INCLUDE_LOADING_BAR_HORIZONTAL */

#ifdef OLED_INCLUDE_LOADING_BAR_ROUND
void oled_loadingBarRound( uint8_t centreX, uint8_t centreY, uint8_t outerRadius, 
    uint8_t innerRadius, uint16_t permille, uint16_t colour, bool hasBorder )
{
    // uint8_t upperBorder[outerRadius*2];
    // uint8_t index = 0;
    // if( hasBorder == true )
    // {
    //     for( uint8_t x = centreX - outerRadius; x <= centreX + outerRadius; ++x )
    //     {
    //         // Use some cartesian sins and coses to find the upper curve. Might only need one quarter
    //         upperBorder[index] = sin(uint8_t) + (uint8_t) cos( (float) something );
    //         ++index;

    //         // might want a polynomial sin and cos actually
    //     }
    // }

    // Fill the outer circle
    if( permille > 250U )
    {
        for( uint8_t x = centreX; x < centreX + outerRadius; ++x )
        {
            for( uint8_t y = centreY - (uint8_t) sqrt( ( outerRadius * outerRadius ) - ( ( x - centreX ) * ( x - centreX ) ) ); y < centreY; ++y )
            {
                oled_setPixel( x, y, colour );
            }
        }
    }
    else
    {
        // deltaX and deltaY used to calculate m in y=mx+c
        uint16_t deltaX = m_intsin( ( permille * 90 ) / 250 );
        uint16_t deltaY = m_intcos( ( permille * 90 ) / 250 );
        for( uint8_t x = centreX; x < centreX + outerRadius; ++x )
        {
            for( uint8_t y = centreY - (uint8_t) sqrt( ( outerRadius * outerRadius ) - ( ( x - centreX ) * ( x - centreX ) ) ); y < centreY - ( ( deltaY * ( x - centreX ) ) / deltaX ); ++y )
            {
                oled_setPixel( x, y, colour );
            }
        }
    }
    
    if( permille > 500U )
    {
        for( uint8_t x = centreX; x < centreX + outerRadius; ++x )
        {
            for( uint8_t y = centreY; y < centreY + (uint8_t) sqrt( ( outerRadius * outerRadius ) - ( ( x - centreX ) * ( x - centreX ) ) ); ++y )
            {
                oled_setPixel( x, y, colour );
            }
        }
    }
    else
    {
        // deltaX and deltaY used to calculate m in y=mx+c
        uint16_t deltaX = m_intsin( ( ( permille - 250 ) * 90 ) / 250 );
        uint16_t deltaY = m_intcos( ( ( permille - 250 ) * 90 ) / 250 );
        bool hasWritten;
        for( uint8_t y = centreY; y < centreY + outerRadius; ++y )
        {

            // for( uint8_t x = centreX - (uint8_t) sqrt( ( outerRadius * outerRadius ) - ( ( y - centreY ) * ( y - centreY ) ) ); x < centreX - ( ( deltaX * ( y - centreY ) ) / deltaY ); ++x )
            hasWritten = false;
            for( uint8_t x = centreX + ( ( deltaX * ( y - centreY ) ) / deltaY ); x < centreX + (uint8_t) sqrt( ( outerRadius * outerRadius ) - ( ( y - centreY ) * ( y - centreY ) ) ); ++x )
            {
                hasWritten = true;
                oled_setPixel( x, y, colour );
                // sleep_ms(20);
            }
            if( hasWritten == false )
                break;
        }
    }
    
    if( permille > 750 )
    {
        for( uint8_t x = centreX - outerRadius; x < centreX; ++x )
        {
            for( uint8_t y = centreY; y < centreY + (uint8_t) sqrt( ( outerRadius * outerRadius ) - ( ( x - centreX ) * ( x - centreX ) ) ); ++y )
            {
                oled_setPixel( x, y, colour );
            }
        }
    }

    
    // Fill the inner circle
    uint8_t yBound;
    if( innerRadius != 0 )
    {
        for( uint8_t x = centreX - innerRadius; x < centreX + innerRadius; ++x )
        {
            yBound = (uint8_t) sqrt( ( innerRadius * innerRadius ) - ( ( x - centreX ) * ( x - centreX ) ) );
            // printf("%d\n", ( innerRadius * innerRadius ) - ( ( x - innerRadius ) * ( x - innerRadius ) ));
            for( uint8_t y = centreY - yBound; y < centreY + yBound; ++y )
            {
                oled_setPixel( x, y, 0x0000 );
            }
        }
    }
}
#endif /* OLED_INCLUDE_LOADING_BAR_ROUND */

#ifdef OLED_INCLUDE_TEST_FUNCTION
void oled_test( void ) // Needs rewriting
{
    uint16_t color = 0;
    uint8_t drawMode = 0;
    uint16_t red;
    uint16_t green;
    uint16_t blue;
    m_chipSelect();
    for( ;; )
    {
        for( uint8_t y = 0U; y < m_displayHeight; ++y )
        {
            for( uint8_t x = 0U; x < m_displayWidth; ++x )
            {
                if( drawMode == 0 )
                    color = ( ( x / 4 ) & 0b11111 ) << ( 5 + 6 );
                    // color = ( (uint16_t) x * (uint16_t) x) + ( (uint16_t) y * (uint16_t) y);
                else if( drawMode == 1 )
                    color = ( ( y / 3 ) & 0b111111 ) << ( 5 );
                    // color = ( (uint16_t) x * (uint16_t) x) - ( (uint16_t) y * (uint16_t) y);
                else if( drawMode == 2)
                    color = ( ( x / 4 ) & 0b11111 );
                else if (drawMode == 3 )
                {
                    red = ( x / 4 ) & 0b11111;
                    green = ( ( x + y ) / 4 ) & 0b111111;
                    blue = ( y / 4 ) & 0b11111;
                    color = ( red << ( 5 + 6 ) ) + ( green << 5 ) + blue;
                }

                m_writeReg(0x15);
                m_writeData(x);
                m_writeData(x);
                m_writeReg(0x75);
                m_writeData(y);
                m_writeData(y);
                // fill!
                m_writeReg(0x5C);   
                
                // The 16 bits are sent in two seperate bytes
                m_writeData(color >> 8);
                m_writeData(color);
            }
        }
        sleep_ms( 1000U );
        ++drawMode;
        if( drawMode == 4 )
            drawMode = 0;
    }
    
}
#endif /* OLED_INCLUDE_TEST_FUNCTION */

#if defined OLED_INCLUDE_FONT8 || defined OLED_INCLUDE_FONT12 || defined OLED_INCLUDE_FONT16 || defined OLED_INCLUDE_FONT20 || defined OLED_INCLUDE_FONT24
void oled_writeChar( uint8_t x, uint8_t y, char character, uint8_t fontSize, uint16_t colour )
{
    tFontTable fontTable;

    if( ( character < 32 ) || ( character > ( 32 + 95 ) ) )
    {
        // Invalid ascii character, draw a blank character
        character = 32;
    }

    if( ( x < 0 ) || ( x > m_displayWidth ) || ( y < 0 ) || ( y > m_displayHeight ) )
    {
        // Out of display bounds
        return;
    }

    switch( fontSize )
    {
#ifdef OLED_INCLUDE_FONT8
        case 8U:
        {
            fontTable = Font8;
        }
        break;
#endif /* OLED_INCLUDE_FONT8 */
#ifdef OLED_INCLUDE_FONT12
        case 12U:
        {
            fontTable = Font12;
        }
        break;
#endif /* OLED_INCLUDE_FONT12 */
#ifdef OLED_INCLUDE_FONT16
        case 16U:
        {
            fontTable = Font16;
        }
        break;
#endif /* OLED_INCLUDE_FONT16 */
#ifdef OLED_INCLUDE_FONT20
        case 20U:
        {
            fontTable = Font20;
        }
        break;
#endif /* OLED_INCLUDE_FONT20 */
#ifdef OLED_INCLUDE_FONT24
        case 24U:
        {
            fontTable = Font24;
        }
        break;
#endif /* OLED_INCLUDE_FONT24 */
        default:
        {
            // Do nothing, font height not supported
            return;
        }
        break;
    }

    // Write the character

    // Calculate the width of a character in bytes
    uint16_t characterWidthBytes = ( fontTable.Width / 8U ) + 1U;
    // Find starting position in the array. Note that the first 32 characters of
    // ascii aren't human readable
    uint16_t arrayPosition = fontTable.Height * characterWidthBytes * (uint16_t) ( character - 32U );
    // bitPosition records the position within each byte
    uint8_t bitPosition;
    for( uint8_t yChar = 0U; yChar < fontTable.Height; yChar++ )
    {
        bitPosition = 0;
        for( uint8_t xChar = 0U; xChar < fontTable.Width; xChar++ )
        {
            if( ( fontTable.table[arrayPosition] & ( 0b10000000 >> bitPosition ) ) != 0 )
                oled_setPixel( x + xChar, y + yChar, colour );

            ++bitPosition;
            if( bitPosition == 8 )
            {
                ++arrayPosition;
                bitPosition = 0U;
            }
        }
        ++arrayPosition;
    }
}

void oled_writeText( uint8_t xStartPos, uint8_t yStartPos, const char text[],
    uint8_t fontSize, uint16_t colour, bool useTextWrapping )
{
    uint8_t xCurrentTextPosition = xStartPos;
    uint8_t yCurrentTextPosition = yStartPos;
    uint8_t characterWidth;
    if( fontSize == 8U )
        characterWidth = OLED_FONT8_WIDTH;
    else if( fontSize == 12U )
        characterWidth = OLED_FONT12_WIDTH;
    else if( fontSize == 16U )
        characterWidth = OLED_FONT16_WIDTH;
    else if( fontSize == 20U )
        characterWidth = OLED_FONT20_WIDTH;
    else if( fontSize == 24U )
        characterWidth = OLED_FONT24_WIDTH;
    else
    {
        // Unsupported font size
        return;
    }

    while( *text != 0 )
    {
        if( useTextWrapping == true )
        {
            // Have we've gone too far to the right
            if( ( xCurrentTextPosition + characterWidth ) > m_displayWidth )
            {
                xCurrentTextPosition = xStartPos;
                yCurrentTextPosition += fontSize + OLED_WRITE_TEXT_CHARACTER_GAP;
            }
            // Have we've gone too far down
            if( ( yCurrentTextPosition + fontSize ) > m_displayHeight )
            {
                // Ran out of space, stop writing text
                break;
            }
        }

        oled_writeChar( xCurrentTextPosition, yCurrentTextPosition, *text, fontSize, colour );
        ++text;
        xCurrentTextPosition += characterWidth + OLED_WRITE_TEXT_CHARACTER_GAP;
    }
}

int oled_terminalInit( uint8_t fontSize, uint16_t colour )
{
    if( m_terminalBitmapState != e_terminalUninitialised )
        return 2; // Fail as terminal already initialised

    switch( fontSize )
    {
#ifdef OLED_INCLUDE_FONT8
        case 8U:
        {
            m_terminalFontTablePtr = &Font8;
        }
        break;
#endif /* OLED_INCLUDE_FONT8 */
#ifdef OLED_INCLUDE_FONT12
        case 12U:
        {
            m_terminalFontTablePtr = &Font12;
        }
        break;
#endif /* OLED_INCLUDE_FONT12 */
#ifdef OLED_INCLUDE_FONT16
        case 16U:
        {
            m_terminalFontTablePtr = &Font16;
        }
        break;
#endif /* OLED_INCLUDE_FONT16 */
#ifdef OLED_INCLUDE_FONT20
        case 20U:
        {
            m_terminalFontTablePtr = &Font20;
        }
        break;
#endif /* OLED_INCLUDE_FONT20 */
#ifdef OLED_INCLUDE_FONT24
        case 24U:
        {
            m_terminalFontTablePtr = &Font24;
        }
        break;
#endif /* OLED_INCLUDE_FONT24 */
        default:
        {
            // Do nothing, font height not supported
            return 3;
        }
        break;
    }

    // Need enough bits to cover the the screen width, might have a few bits unused per row
    // This is so that bytes for each row align, which makes scrolling much much easier
    uint16_t bitmapArraySize = (uint16_t) m_displayWidth / 8U;
    // Round up if needed
    if( ( (uint16_t) m_displayWidth / 8U ) != 0U )
        ++bitmapArraySize;
    // Now need this many bytes per row
    bitmapArraySize *= m_displayHeight;

    TEMP_callocSize = bitmapArraySize; // TEMP

    m_terminalBitmapPtr1 = (uint8_t*) calloc( bitmapArraySize, sizeof( uint8_t ) );
    if( m_terminalBitmapPtr1 == NULL )
        return 1; // Memory allocation failed

    m_terminalBitmapPtr2 = (uint8_t*) calloc( bitmapArraySize, sizeof( uint8_t ) );
    if( m_terminalBitmapPtr1 == NULL )
    {
        // Memory allocation failed
        free( m_terminalBitmapPtr1 );
        m_terminalBitmapPtr1 = NULL;
        return 1;
    }

    m_terminalFontSize = fontSize;
    m_terminalFontColour = colour;
    m_terminalCurrentLine = 0U;
    m_terminalBitmapState = e_terminalBitmap1Next;

    return 0; // Success
}

void oled_terminalWrite( const char text[] )
{
    uint8_t* bitmapPtr;
    bitmapPtr = ( m_terminalBitmapState == e_terminalBitmap1Next ) ? m_terminalBitmapPtr1 : m_terminalBitmapPtr2;

    m_terminalPushBitmap();

    // Have we ran out of vertical space and need to start scrolling?
    uint8_t terminalHeightInLines = m_displayHeight / m_terminalFontSize;
    if( m_terminalCurrentLine == terminalHeightInLines )
    {
        uint8_t bytesPerRow = m_displayWidth / 8U;
        if( ( m_displayWidth % 8U ) != 0 )
            ++bytesPerRow;
        
        uint16_t bitmapSize = bytesPerRow * m_displayHeight;

        uint16_t sourceByte = 0U;
        // Shift everything up
        while( ( sourceByte + (uint16_t) bytesPerRow ) < bitmapSize )
        {
            bitmapPtr[sourceByte] = bitmapPtr[sourceByte + bytesPerRow];
            ++sourceByte;
        }

        // Erase the bottom of the bitmap
        sourceByte = ( m_displayHeight - m_terminalFontSize ) * m_displayWidth;
        while( sourceByte < bitmapSize )
        {
            bitmapPtr[sourceByte] = 0x00; // Background colour
            ++sourceByte;
        }
    }
    
    // --- Add the bitmap to the display ---
    uint8_t xCurrentTextPosition = 0U;
    uint8_t yCurrentTextPosition = m_terminalCurrentLine * m_terminalFontSize;
    uint8_t characterWidth = m_terminalFontTablePtr->Width;
    // uint8_t pixelPositionX;
    // uint8_t pixelPositionY;

    // // Terminal init function ensures the font size is valid
    // // Get character width
    // if( m_terminalFontSize == 8U )
    //     characterWidth = OLED_FONT8_WIDTH;
    // else if( m_terminalFontSize == 12U )
    //     characterWidth = OLED_FONT12_WIDTH;
    // else if( m_terminalFontSize == 16U )
    //     characterWidth = OLED_FONT16_WIDTH;
    // else if( m_terminalFontSize == 20U )
    //     characterWidth = OLED_FONT20_WIDTH;
    // else if( m_terminalFontSize == 24U )
    //     characterWidth = OLED_FONT24_WIDTH;
    
    // Put the characters on the bitmap
    while( *text != 0 )
    {
        // Check if we've gone too far to the right
        if( ( xCurrentTextPosition + characterWidth ) > m_displayWidth )
        {
            // No text wrapping implemented for the terminal so just stop
            break;
        }
        
        // Write the character
        m_terminalWriteChar( *text, xCurrentTextPosition, yCurrentTextPosition );

        ++text;
        xCurrentTextPosition += characterWidth + OLED_WRITE_TEXT_CHARACTER_GAP;
    }

    // Push the bitmap
    m_terminalPushBitmap();

    // Update variables
    if( m_terminalCurrentLine != terminalHeightInLines )
        ++m_terminalCurrentLine;
}

void oled_terminalWriteTemp( const char text[] ) {}

void oled_terminalClear( void ) {}

void oled_terminalDeinit( void ) {}
#endif /* defined OLED_INCLUDE_FONT8 || defined OLED_INCLUDE_FONT12 || defined OLED_INCLUDE_FONT16 || defined OLED_INCLUDE_FONT20 || defined OLED_INCLUDE_FONT24 */

/*
 * Function: m_terminalWriteChar
 * --------------------
 * Write a char to the terminal bitmap
 *
 * bitmapPtr: Pointer to the bitmap to be written to
 * fontTablePtr: Pointer to the relevant font table
 * 
 *
 * returns: void
 */
static inline void m_terminalWriteChar( char character, uint8_t textOriginX,
    uint8_t textOriginY )
{
    uint8_t* bitmapPtr = ( m_terminalBitmapState == e_terminalBitmap1Next ) ? m_terminalBitmapPtr1 : m_terminalBitmapPtr2;
    // Calculate the width of a character in bytes
    uint8_t characterWidthBytes = ( (uint8_t) m_terminalFontTablePtr->Width / 8U ) + 1U;
    // Find starting position in the array. Note that the first 32 characters of
    // ascii aren't human readable
    uint16_t tablePosition = m_terminalFontTablePtr->Height * (uint16_t) characterWidthBytes * (uint16_t) ( character - 32U );
    // tableBitPosition records the position within each byte, for the font table
    uint8_t tableBitPosition;

    // Find width of a row on the display in bytes for the bitmap
    uint8_t bitmapRowWidthBytes = m_displayWidth / 8U;
    // Round up if needed
    if( ( (uint16_t) m_displayWidth % 8U ) != 0U )
        ++bitmapRowWidthBytes;
    
    uint16_t bitmapBytePosition;

    for( uint8_t yPosition = 0U; yPosition < m_terminalFontTablePtr->Height; yPosition++ )
    {
        tableBitPosition = 0U;
        for( uint8_t xPosition = 0U; xPosition < m_terminalFontTablePtr->Width; xPosition++ )
        {
            // Check the bit in the font table
            if( ( m_terminalFontTablePtr->table[tablePosition] & ( 0b10000000 >> tableBitPosition ) ) != 0 )
            {
                // Find the bit within the bitmap that needs to be set high
                bitmapBytePosition = bitmapRowWidthBytes * ( textOriginY + yPosition );
                bitmapBytePosition += ( textOriginX + xPosition ) / 8U;
                // Set the pixel in the bitmap high

                bitmapPtr[bitmapBytePosition] |= 0b1 << ( ( textOriginX + xPosition ) % 8U);
            }

            ++tableBitPosition;
            if( tableBitPosition == 8 )
            {
                ++tablePosition;
                tableBitPosition = 0U;
            }
        }
        ++tablePosition;
    }

}

/*
 * Function: m_terminalPushBitmap
 * --------------------
 * Push the current terminal bitmap to the screen. This function does
 * change the value of m_terminalBitmapState
 *
 * parameters: none
 *
 * returns: void
 */
void m_terminalPushBitmap( void ) 
{
    // Position within each byte in the bitmap array
    uint8_t bitPosition = 0U;
    uint16_t byteNumber = 0U;
    // Position on the display
    uint8_t displayPositionX = 0U;
    uint8_t displayPositionY = 0U;
    // Check which bitmap to push
    uint8_t* bitmap = ( m_terminalBitmapState == e_terminalBitmap1Next ) ? m_terminalBitmapPtr1 : m_terminalBitmapPtr2;

    while( displayPositionY != m_displayHeight )
    {

        if( ( bitmap[byteNumber] & ( 1 << bitPosition ) ) == 0 )
        {
            oled_setPixel( displayPositionX, displayPositionY, 0x0000U ); // Background colour
        }
        else
        {
            oled_setPixel( displayPositionX, displayPositionY, m_terminalFontColour ); // Text colour
        }
        
        ++bitPosition;
        if( bitPosition == 8U )
        {
            bitPosition = 0U;
            ++byteNumber;
        }
        ++displayPositionX;
        if( displayPositionX == m_displayWidth )
        {
            displayPositionX = 0U;
            ++displayPositionY;
        }
    }

    // Change the next bitmap value
    if( m_terminalBitmapState == e_terminalBitmap1Next )
        m_terminalBitmapState = e_terminalBitmap2Next;
    else
        m_terminalBitmapState = e_terminalBitmap1Next;
}

/*
 * Function: m_displayInit
 * --------------------
 * Initialise the display by writing data to registers
 *
 * parameters: none
 *
 * returns: void
 */
static inline void m_displayInit( void )
{
    // CS should be set low (active) prior to calling this function
    // Now for some inexplicable magic from the manufacturer. Comments here are from the manufacturer
    m_writeReg(0xFD);  // Command lock
    m_writeData(0x12);
    m_writeReg(0xFD);  // Command lock
    m_writeData(0xB1);

    m_writeReg(0xAE);  // Display off
    m_writeReg(0xA4);  // Normal Display mode

    m_writeReg(0x15);  // Set column address
    m_writeData(0x00); // Column address start 00
    m_writeData(0x7F); // Column address end 127
    m_writeReg(0x75);  // Set row address
    m_writeData(0x00); // Row address start 00
    m_writeData(0x7F); // Row address end 127    

    m_writeReg(0xB3);
    m_writeData(0xF1);

    m_writeReg(0xCA);  
    m_writeData(0x7F);

    m_writeReg(0xA0);  // Set re-map & data format
    m_writeData(0x74); // Horizontal address increment

    m_writeReg(0xA1);  // Set display start line
    m_writeData(0x00); // Start 00 line

    m_writeReg(0xA2);  // Set display offset
    m_writeData(0x00);

    m_writeReg(0xAB);  
    m_writeReg(0x01);  

    m_writeReg(0xB4);  
    m_writeData(0xA0);   
    m_writeData(0xB5);  
    m_writeData(0x55);    

    m_writeReg(0xC1);  
    m_writeData(0xC8); 
    m_writeData(0x80);
    m_writeData(0xC0);

    m_writeReg(0xC7);  
    m_writeData(0x0F);

    m_writeReg(0xB1);  
    m_writeData(0x32);

    m_writeReg(0xB2);  
    m_writeData(0xA4);
    m_writeData(0x00);
    m_writeData(0x00);

    m_writeReg(0xBB);  
    m_writeData(0x17);

    m_writeReg(0xB6);
    m_writeData(0x01);

    m_writeReg(0xBE);
    m_writeData(0x05);

    m_writeReg(0xA6);
}

/*
 * Function: m_chipSelect
 * --------------------
 * Select the display as an SPI device by setting CS to low
 *
 * parameters: none
 *
 * returns: void                                                                         
 */
static inline void m_chipSelect( void )
{
    gpio_put( PICO_DEFAULT_SPI_CSN_PIN, 0 );
}

/*
 * Function: m_chipDeselect
 * --------------------
 * Deselct the display as an SPI device by setting CS to high
 *
 * parameters: none
 *
 * returns: void
 */
static inline void m_chipDeselect( void )
{
    gpio_put( PICO_DEFAULT_SPI_CSN_PIN, 1 );
}

/*
 * Function: m_writeReg
 * --------------------
 * Select register to write data to
 *
 * parameters: none
 *
 * returns: void
 */
static inline void m_writeReg( uint8_t reg )
{
    gpio_put( m_dcPin, 0 );
    if( m_spiInstance == 0 )
        spi_write_blocking( spi0, &reg, 1 );
    else
        spi_write_blocking( spi1, &reg, 1 );
}

/*
 * Function: m_writeData
 * --------------------
 * Write data to the selected register
 *
 * parameters: none
 *
 * returns: void
 */
static inline void m_writeData( uint8_t data )
{
    gpio_put( m_dcPin, 1 );
    if( m_spiInstance == 0 )
        spi_write_blocking( spi0, &data, 1 );
    else
        spi_write_blocking( spi1, &data, 1 );
}

#ifdef OLED_INCLUDE_LOADING_BAR_ROUND
/*
 * Function: m_intsin
 * --------------------
 * Calculate sin using m_intcos
 *
 * angle: Angle in degrees
 *
 * returns: round( 1000 * cos( angle ) )
 */
static inline int16_t m_intsin( int16_t angle )
{
    return m_intcos( angle - 90 );
}

/*
 * Function: m_intcos
 * --------------------
 * Calculate cos using a lookup table and integer math. Uses no floats/doubles
 *
 * angle: Angle in degrees
 *
 * returns: round( 1000 * sin( angle ) )
 */
static inline int16_t m_intcos( int16_t angle )
{
    // Take abs of angle
    int16_t newAngle = ( angle < 0 ) ? ( -1 * angle ) : angle;
    // Move the angle to within 0 and 360
    newAngle = newAngle - ( ( newAngle / 360 ) * 360 );

    int8_t quadrant = newAngle / 90;
    if( quadrant == 0 ) // ( newAngle >= 0 ) and ( newAngle < 90 )
        return cosLookupTable[newAngle];
    else if( quadrant == 1 ) // ( newAngle >= 90 ) and ( newAngle < 180 )
        return -1 * cosLookupTable[180 - newAngle];
    else if( quadrant == 2 ) // ( newAngle >= 180 ) and ( newAngle < 270 )
        return -1 * cosLookupTable[newAngle - 180];
    else // quadrant = 3, ( newAngle >= 270 ) and ( newAngle < 360 )
        return cosLookupTable[360 - newAngle];
}
#endif /* OLED_INCLUDE_LOADING_BAR_ROUND */
