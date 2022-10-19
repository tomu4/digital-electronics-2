/***********************************************************************
 * 
 * Stopwatch by Timer/Counter2 on the Liquid Crystal Display (LCD)

 * ATmega328P (Arduino Uno), 16 MHz, PlatformIO
 *
 * Copyright (c) 2017 Tomas Fryza
 * Dept. of Radio Electronics, Brno University of Technology, Czechia
 * This work is licensed under the terms of the MIT license.
 * 
 * Components:
 *   16x2 character LCD with parallel interface
 *     VSS  - GND (Power supply ground)
 *     VDD  - +5V (Positive power supply)
 *     Vo   - (Contrast)
 *     RS   - PB0 (Register Select: High for Data transfer, Low for Instruction transfer)
 *     RW   - GND (Read/Write signal: High for Read mode, Low for Write mode)
 *     E    - PB1 (Read/Write Enable: High for Read, falling edge writes data to LCD)
 *     D3:0 - NC (Data bits 3..0, Not Connected)
 *     D4   - PD4 (Data bit 4)
 *     D5   - PD5 (Data bit 5)
 *     D6   - PD6 (Data bit 3)
 *     D7   - PD7 (Data bit 2)
 *     A+K  - Back-light enabled by PB2
 * 
 **********************************************************************/

/* Includes ----------------------------------------------------------*/
#include <avr/io.h>         // AVR device-specific IO definitions
#include <avr/interrupt.h>  // Interrupts standard C library for AVR-GCC
#include <gpio.h>           // GPIO library for AVR-GCC
#include "timer.h"          // Timer library for AVR-GCC
#include <lcd.h>            // Peter Fleury's LCD library
#include <stdlib.h>         // C library. Needed for number conversions


/* Function definitions ----------------------------------------------*/
/**********************************************************************
 * Function: Main function where the program execution begins
 * Purpose:  Update stopwatch value on LCD screen when 8-bit 
 *           Timer/Counter2 overflows.
 * Returns:  none
 **********************************************************************/
/* Global variables --------------------------------------------------*/
// Custom character definition using https://omerk.github.io/lcdchargen/
uint8_t stick_figure[40] = {
	0b00100,0b01110,0b01110,0b00100,0b11111,0b00100,0b01010,0b10001,

  0b00000,0b00000,0b00000,0b00011,0b00111,0b00111,0b00111,0b00011,

  0b00000,0b00000,0b00000,0b00000,0b00000,0b01110,0b11111,0b11111,

  0b11111,0b11111,0b01110,0b00000,0b00000,0b00000,0b00000,0b00000,

  0b11000,0b11100,0b11100,0b11100,0b11000,0b00000,0b00000,0b00000
};

int main(void)
{
    // Initialize LCD display
    lcd_init(LCD_DISP_ON);

    // Set pointer to beginning of CGRAM memory
    lcd_command(1 << LCD_CGRAM);
    for (uint8_t i = 0; i < 40; i++)
    {
        // Store all new chars to memory line by line
        lcd_data(stick_figure[i]);
    }
    // Set DDRAM address to display characters
    lcd_command(1 << LCD_DDRAM);

    


    // Put string(s) on LCD screen
    lcd_gotoxy(1, 0);
    lcd_puts("00:00.0");
    lcd_gotoxy(11,0);
    lcd_putc('a');
    lcd_gotoxy(1,1);
    lcd_putc('b');
    lcd_gotoxy(11,1);
    lcd_putc('c');
    lcd_gotoxy(6,1);
    lcd_putc(0);
    lcd_gotoxy(14,0);
    lcd_putc(0x01);
    lcd_gotoxy(15,0);
    lcd_putc(0x02);
    lcd_gotoxy(14,1);
    lcd_putc(0x03);
    lcd_gotoxy(15,1);
    lcd_putc(0x04);

    GPIO_mode_output(&DDRB, PB2);
    GPIO_write_high(&PORTB, PB2);

    // Configuration of 8-bit Timer/Counter2 for Stopwatch update
    // Set the overflow prescaler to 16 ms and enable interrupt

    TIM2_overflow_16ms();
    TIM2_overflow_interrupt_enable();

    // Enables interrupts by setting the global interrupt mask
    sei();

    // Infinite loop
    while (1)
    {
        /* Empty loop. All subsequent operations are performed exclusively 
         * inside interrupt service routines, ISRs */
    }

    // Will never reach this
    return 0;
}

/* Interrupt service routines ----------------------------------------*/
/**********************************************************************
 * Function: Timer/Counter2 overflow interrupt
 * Purpose:  Timer.
 **********************************************************************/
ISR(TIMER2_OVF_vect)
{
    static uint8_t no_of_overflows = 0;
    static uint8_t tenths = 0;
    static uint8_t secs = 0;
    static uint8_t mins = 0;
    
    char string[2];             // String for converted numbers by itoa()

    no_of_overflows++;
    if (no_of_overflows >= 6)
    {
        // Do this every 6 x 16 ms = 100 ms
        no_of_overflows = 0;

        // Count tenth of seconds 0, 1, ..., 9, 0, 1, ...
        tenths++;
        if(tenths>=10){
            secs++;
            tenths = 0;
        }
        if(secs>=60){
            mins++;
            secs = 0;
            lcd_gotoxy(4, 0);
            lcd_puts("00");
        }

        if(mins>=60){
            mins = 0;
            lcd_gotoxy(1, 0);
            lcd_puts("00");

        }

        itoa(tenths, string, 10);  // Convert decimal value to string
        lcd_gotoxy(7, 0);
        lcd_puts(string);
        itoa(secs, string, 10);  // Convert decimal value to string
        if(secs>=10){
          lcd_gotoxy(4, 0);
        }
        else{
          lcd_gotoxy(5, 0);
        }
        lcd_puts(string);
        itoa(mins, string, 10);  // Convert decimal value to string
        if(mins>=10){
          lcd_gotoxy(1, 0);
        }
        else{
          lcd_gotoxy(2, 0);
        }
        lcd_puts(string);
    }
}