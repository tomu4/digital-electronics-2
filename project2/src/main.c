/***********************************************************************
 * 
 * Application of analog joy-stick (2 ADC channels, 1 push button), 
 * rotary encoder, and Digilent PmodCLP LCD module.
 * 
 * ATmega328P (Arduino Uno), 16 MHz, PlatformIO
 *
 * Copyright (c) 2022 Tomas Uchytil
 * Dept. of Radio Electronics, Brno University of Technology, Czechia
 * This work is licensed under the terms of the MIT license.
 * 
 **********************************************************************/


/* Includes ----------------------------------------------------------*/
#include <avr/io.h>         // AVR device-specific IO definitions
#include <avr/interrupt.h>  // Interrupts standard C library for AVR-GCC
#define __DELAY_BACKWARD_COMPATIBLE__ //delay library didnt work without this line
#include <util/delay.h>     // Delay library
#include <gpio.h>           // GPIO library for AVR-GCC
#include "timer.h"          // Timer library for AVR-GCC
#include <lcd.h>            // Peter Fleury's LCD library
#include <stdlib.h>         // C library. Needed for number conversions
#include <uart.h>           // UART serial communication library. For debugging purposes.

/* Variable definitions ----------------------------------------------*/
static uint8_t cycle=0; //swithing between ADC channels
static uint16_t memory_x=0; //stores x value between analog readings
static uint16_t memory_y=0; //stores y value between analog readings
static uint8_t ovf_count = 0; //Timer divider, up to 20
static uint16_t x_time=0; //stores analog input 0 value converted to time for which servo 1 output needs to be high
static uint16_t y_time=0; //stores analog input 1 value converted to time for which servo 2 output needs to be high
char string[4];

int main(void)
{
    uart_init(UART_BAUD_SELECT(9600, F_CPU)); //for debugging purposes

    // Configure Analog-to-Digital Convertion unit
    // Select ADC voltage reference to "AVcc with external capacitor at AREF pin"
    ADMUX = ADMUX | (1<<REFS0);

    // Select input channel ADC0 (voltage divider pin)
    ADMUX = ADMUX & ~(1<<MUX3 | 1<<MUX2 | 1<< MUX1 | 1<< MUX0);

    // Enable ADC module
    ADCSRA = ADCSRA | (1<<ADEN);

    // Enable conversion complete interrupt
    ADCSRA |= (1<<ADIE);

    // Set clock prescaler to 128
    ADCSRA |= (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);

    // Configure 8-bit Timer/Counter2 to start ADC conversion
    // Set prescaler to 16 ms and enable overflow interrupt
    TIM2_overflow_16ms();
    TIM2_overflow_interrupt_enable();
    // Configure 8-bit Timer/Counter0 to start ADC conversion
    // Set prescaler to 1 ms and enable overflow interrupt
    TIM0_overflow_1ms();
    TIM0_overflow_interrupt_enable();

    GPIO_mode_output(&DDRD,PIN5);
    GPIO_mode_output(&DDRD,PIN3);
    // Enables interrupts by setting the global interrupt mask
    sei();
    // Infinite loop
    while (1)
    {
        /* Empty loop. All subsequent operations are performed exclusively 
         * inside interrupt service routines ISRs */
    }

    // Will never reach this
    return 0;
}


/* Interrupt service routines ----------------------------------------*/
/**********************************************************************
 * Function: Timer/Counter1 overflow interrupt
 * Purpose:  Use single conversion mode and start conversion every 100 ms.
 **********************************************************************/
ISR(TIMER2_OVF_vect)
{
  static uint8_t ovf_count_2=0;
  if(cycle==0&&ovf_count_2>2){
      //select Analog input 0
      ADMUX = ADMUX & ~(1<<MUX3 | 1<<MUX2 | 1<< MUX1 | 1<< MUX0);
      ADCSRA |= (1<<ADSC);//start ADC conversion
      cycle=1;//next time read analog input 1
      ovf_count_2=0;//reset overflow counter
    }
    else if(cycle==1&&ovf_count_2>2){
      //select Analog input 1
      ADMUX = ADMUX & ~(1<<MUX3 | 1<<MUX2 | 1<< MUX1);
      ADMUX = ADMUX | (1<<MUX0);
      ADCSRA |= (1<<ADSC);// Start ADC conversion
      cycle=0;//next time read analog input 0
      ovf_count_2=0;//reset overflow counter
      

    }
  ovf_count_2++;
    
}

/**********************************************************************
 * Function: ADC complete interrupt
 * Purpose:  Display converted value on LCD screen.
 **********************************************************************/

ISR(ADC_vect)
{    
    // Read converted value, and assign it to x or y axis, depending on input that was read
    if(cycle==1){memory_x=ADC;}
    else {memory_y=ADC;}
    
}

ISR(TIMER0_OVF_vect)
{
  if(ovf_count>(20)) { //period of servo impulses is aprox. 20ms (=20*1ms timer overflow)
    ovf_count=0; //reset OVF counter
    x_time=650+(memory_x*1.6/10)*10; //convert ADC value to use full range of servo (found experimentally)
    y_time=650+(memory_y*1.6/10)*10; //convert ADC value to use full range of servo (found experimentally)
    

    GPIO_write_high(&PORTD,PIN5);//each 20ms turn on both servo pins
    GPIO_write_high(&PORTD,PIN3);//each 20ms turn on both servo pins
    if(x_time<y_time)
    {
      _delay_us(x_time);//wait for shorter time delay
      GPIO_write_low(&PORTD, PIN5);//turn off corresponding servo output
      _delay_us(y_time-x_time);//wait for rest of longer time delay
      GPIO_write_low(&PORTD, PIN3);//turn off other servo output
    }
    else{
      _delay_us(y_time);//wait for shorter time delay
      GPIO_write_low(&PORTD, PIN3);//turn off corresponding servo output
      _delay_us(x_time-y_time);//wait for rest of longer time delay
      GPIO_write_low(&PORTD, PIN5);//turn off other servo output
    }
    
  }
  ovf_count++;//every 1ms, increase OVF counter
}