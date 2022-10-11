/* Defines -----------------------------------------------------------*/
#define LED_GREEN PB5  // Arduino Uno on-board LED
#define LED_RED PB0    // External active-low LED


/* Includes ----------------------------------------------------------*/
#include <avr/io.h>         // AVR device-specific IO definitions
#include <avr/interrupt.h>  // Interrupts standard C library for AVR-GCC
#include <gpio.h>           // GPIO library for AVR-GCC
#include "timer.h"          // Timer library for AVR-GCC


/* main function ------------------------------------------------------*/
int main(void)
{
    // Set pins where LEDs are connected as output
    GPIO_mode_output(&DDRB, LED_GREEN);
    GPIO_mode_output(&DDRB, LED_RED);

    // Enable overflow interrupt
    TIM1_overflow_262ms();
    TIM1_overflow_interrupt_enable();


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
 * Function: Timer/Counter1 overflow interrupt
 * Purpose:  Toggle on-board LED.
 **********************************************************************/
ISR(TIMER1_OVF_vect)
{
    PORTB = PORTB ^ (1<<LED_GREEN);
}

/* Interrupt service routines ----------------------------------------*/
/**********************************************************************
 * Function: Timer/Counter2 overflow interrupt
 * Purpose:  Toggle external LED.
 **********************************************************************/
ISR(TIMER2_OVF_vect)
{
    static uint8_t no_of_owerflows = 0;

    no_of_owerflows++;
    if(no_of_owerflows>=30){
      no_of_owerflows=0;
      PORTB = PORTB ^ (1<<LED_RED);
    }

    TCNT2 = 128;
}