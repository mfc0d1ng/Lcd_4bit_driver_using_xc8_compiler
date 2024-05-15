#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <xc.h>
#include <proc/pic18f4620.h>

#define _XTAL_FREQ  4000000UL

typedef unsigned char uint8;

#define LCD_CLEAR                0x01
#define LCD_CURSOR_HOME          0x02
#define LCD_WRITE_ORDINARY       0x06
#define LCD_ENABLE_DISPLAY	 0x0c
#define LCD_8BIT_MODE		 0x38
#define LCD_4BIT_MODE		 0x28
#define LCD_DDRAM                0x80

#define LCD_RS 		 LATCbits.LC0
#define LCD_EN		 LATCbits.LC1
#define LCD_D4		 LATCbits.LC2
#define LCD_D5		 LATCbits.LC3
#define LCD_D6		 LATCbits.LC4
#define LCD_D7		 LATCbits.LC5

/**
 * @brief Writes a chunk of data on the data pins.
 * @param __chunk A Chunk of data to be written 
 * 	          on the data pins.
 */
void write(uint8 __chunk)
{
    LCD_D4 = (__chunk)      & 0x01;
    LCD_D5 = (__chunk >> 1) & 0x01;
    LCD_D6 = (__chunk >> 2) & 0x01;
    LCD_D7 = (__chunk >> 3) & 0x01;
}

/**
 * @brief Sends enable pulse to lcd to allow data transmission.
 */
void send_enable(void)
{
    LCD_EN = 0x1;
    __delay_us(5);
    LCD_EN = 0x0;
}

/**
 * @brief Sends a command to lcd.
 * @param __cmd A Command to be sent.
 */
void lcd_cmd(uint8 __cmd)
{
    LCD_RS = 0x0;
    write(__cmd >> 4);
    send_enable();
    write(__cmd);
    send_enable();
}

/**
 * @brief Initializes lcd.
 */
void lcd_init(void)
{
    /* The digital pins connected to lcd pins must be output pins */
    TRISC = 0xc0;
    __delay_ms(20);
    lcd_cmd(LCD_8BIT_MODE);
    __delay_ms(5);
    lcd_cmd(LCD_8BIT_MODE);
    __delay_ms(150);
    lcd_cmd(LCD_8BIT_MODE);
    lcd_cmd(LCD_CLEAR);
    lcd_cmd(LCD_CURSOR_HOME);
    lcd_cmd(LCD_WRITE_ORDINARY);
    lcd_cmd(LCD_ENABLE_DISPLAY);
    lcd_cmd(LCD_4BIT_MODE);
    lcd_cmd(LCD_DDRAM);
}

/**
 * @brief Sets the cursor at the specified row and column.
 * @param __row	Row number.
 * @param __col Column number.
 */
void lcd_cursor(uint8 __row, uint8 __col)
{
    --__col;
    switch (__row)
    {
    case 1:
        lcd_cmd(0x80 + __col);
        break;
    case 2:
        lcd_cmd(0xc0 + __col);
        break;
    default:;
    }	
}

/**
 * @brief Sends a byte of data to lcd.
 * @param __data A byte of data to be sent.
 */
void lcd_data(uint8 __data)
{
    LCD_RS = 0x1;
    write(__data >> 4);
    send_enable();
    write(__data);
    send_enable();
}

/**
 * @brief Displays a character at the current cursor position.
 * @param __char A character to be displayed.
 */
void lcd_char(uint8 __char)
{
	lcd_data(__char);
}

/**
 * @brief Displays a string beginning from the current cursor position.
 * @param __str A string to be displayed.
 */
void lcd_str(const uint8* __str)
{
    while (*__str)
    {
        lcd_data(*__str++);
    }	
}

/**
 * @brief Displays a string beginning from the specified row and column. 
 * @param __row	Row number.
 * @param __col Column number.
 * @param __str A string to be displayed.
 */
void lcd_out(uint8 __row, uint8 __col, const uint8* __str)
{
    lcd_cursor(__row, __col);
    lcd_str(__str);
}

/**
 * @brief Converts a decimal value to string.
 * @param __buffer Suitable buffer for holding the string.
 * @param __deimal A Decimal value to be converted.
 */
void uint8_toString(uint8* __buffer, uint8 __deimal)
{
    sprintf(__buffer, "%i", __deimal);
}

int main()
{	

    lcd_init();

    const uint8* msg = "Let's display the sizes of the standard "
		       "data types regarding this C compiler!";

    for (uint8 i = 0; msg[i] != '\0'; ++i)
    {
        if(i == 40)
        {
            lcd_cursor(2, 1);
        }
        lcd_char(msg[i]);
        __delay_ms(60);
    }
	
    __delay_ms(3000);

    uint8 buffer[3];

    const uint8 std_types_sizes[] = {
                                       sizeof(signed char),
                                       sizeof(unsigned char),
                                       sizeof(signed short),
                                       sizeof(unsigned short),
                                       sizeof(signed int),
                                       sizeof(unsigned int),
                                       sizeof(signed long int), 
                                       sizeof(unsigned long int),
                                       sizeof(signed long long int)
                                       sizeof(unsigned long long int),
                                       sizeof(float),
                                       sizeof(double)
				     };

    const uint8* texts[] = {
                              "size of (signed char): ",
                              "size of (unsigned char): ",
                              "size of (signed short): ",
                              "size of (unsigned short): ",
                              "size of (signed int): ",
                              "size of (unsigned int): ",
                              "size of (signed long int): ",
                              "size of (unsigned long int): ",
                              "size of (signed long long int): ",
                              "size of (unsigned long long int): ",
                              "size of (float): ",
                              "size of (double): "
			   };

    while (true)
    {
        for (uint8 i = 0; i < 12; ++i)
        {
	    lcd_cmd(LCD_CLEAR);
	    __delay_ms(250);
	    uint8_toString(buffer, std_types_sizes[i]);
	    lcd_out(1, 5, texts[i]);
	    lcd_out(2, 10, buffer);
	    lcd_out(2, 12, "bytes");
	    __delay_ms(3000);
        }
    }

}

