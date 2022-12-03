/***********************************************************************
 * 
 * Use Analog-to-digital conversion to read push buttons on LCD keypad
 * shield and display it on LCD screen.
 * 
 * ATmega328P (Arduino Uno), 16 MHz, PlatformIO
 *
 * Copyright (c) 2018 Tomas Fryza
 * Dept. of Radio Electronics, Brno University of Technology, Czechia
 * This work is licensed under the terms of the MIT license.
 * 
 **********************************************************************/


/* Includes ----------------------------------------------------------*/
#include <avr/io.h>         // AVR device-specific IO definitions
#include <avr/interrupt.h>  // Interrupts standard C library for AVR-GCC
#include <gpio.h>           // GPIO library for AVR-GCC
#include "timer.h"          // Timer library for AVR-GCC
#include <lcd.h>            // Peter Fleury's LCD library
#include <stdlib.h>         // C library. Needed for number conversions
#include <uart.h>

/* Function definitions ----------------------------------------------*/
/**********************************************************************
 * Function: Main function where the program execution begins
 * Purpose:  Use Timer/Counter1 and start ADC conversion every 100 ms.
 *           When AD conversion ends, send converted value to LCD screen.
 * Returns:  none
 **********************************************************************/
static uint8_t cycle=0;
static uint8_t diff=3;
static uint16_t memory_x=0;
static uint16_t memory_y=0;
static uint8_t int_zero=0;
static uint8_t int_one=0;
static uint16_t score=0;
static uint16_t high_score=0;
static uint8_t x_key_res = 0;
static uint8_t y_key_res = 0;
static uint8_t button_mem = 0;
char string[1];
char string3[3];
static uint8_t game_status=0; //0 = game stopped, 1 = game runs, 2 = choose difficulty
static uint8_t current_char=0;
uint8_t customChar[] = {
  //arrow up
  0b00000,0b00100,0b01110,0b10101,0b00100,0b00100,0b00100,0b00000,
  //arrow right
  0b00000,0b00100,0b00010,0b11111,0b00010,0b00100,0b00000,0b00000,
  //arrow down
  0B00000,0B00100,0B00100,0B00100,0B10101,0B01110,0B00100,0B00000,
  //arrow left
  0b00000,0b00100,0b01000,0b11111,0b01000,0b00100,0b00000,0b00000,
  //tick
  0b00000,0b00001,0b00011,0b10110,0b11100,0b01000,0b00000,0b00000,
  //star (for high score)
  0b00000,0b00100,0b10101,0b01110,0b10101,0b00100,0b00000,0b00000
};

uint8_t random_int(uint8_t lower, uint8_t upper)
{
  srand(rand());
  return (lower + rand() / (RAND_MAX / (upper - lower + 1) + 1));
}

void show_char(void)
{
  lcd_gotoxy(0,0);
  lcd_puts("                ");
  lcd_gotoxy(2,0);
  current_char=random_int(0,3);
  lcd_putc(current_char);
  //lcd_putc(48+current_char);
  lcd_gotoxy(2,1);lcd_puts("   ");lcd_gotoxy(2,1);
  itoa((score),string3,10);
  lcd_puts(string3);
}

void start_game(void)
{
  score=0;
  game_status=1;
  show_char();
  TIM1_overflow_4s();
  TCNT1=0+(65536/6*(diff-1));
}

void end_game()
{
  TIM1_stop();
  game_status = 0;
  lcd_gotoxy(0,0);
  lcd_puts("                ");
  lcd_gotoxy(0,0);
  lcd_puts("GAME OVER! [OK]");
  if(score>=high_score) {high_score=score;}
  lcd_gotoxy(12,1);lcd_puts("   ");lcd_gotoxy(12,1);
  itoa((high_score),string3,10);
  lcd_puts(string3);
}

void move_check(uint8_t key)
{
  TIM1_stop();
  if(key==current_char)
  {
    score=score+1;
    show_char();
    TIM1_overflow_4s();
    TCNT1=0+(65536/5*(diff-1));
  }
  else
  {
    end_game();
  }
}

void button_press()
{
  if (game_status==0)
  {
    game_status=2;
    lcd_gotoxy(0,0);
    lcd_puts("                ");
    lcd_gotoxy(0,0);
    lcd_puts(" difficulty: 3 ");
    diff=3;
  }
  else if (game_status==2)
  {
    start_game();
  }
}

void counter_CW(void)
{
  int_zero=0;
  int_one=0;
  if(game_status==2 && diff>1)
  {
    diff--;
    lcd_gotoxy(13,0);
    lcd_puts("  ");
    itoa(diff,string,10);
    lcd_gotoxy(13,0);
    lcd_puts(string);
  }
}

void CW(void)
{
  int_zero=0;
  int_one=0;
  if(game_status==2 && diff<5)
  {
    diff++;
    lcd_gotoxy(13,0);
    lcd_puts("  ");
    itoa(diff,string,10);
    lcd_gotoxy(13,0);
    lcd_puts(string);
  }
}

int main(void)
{
    // Initialize display
    lcd_init(LCD_DISP_ON);
    uart_init(UART_BAUD_SELECT(9600, F_CPU));

    // Set pointer to beginning of CGRAM memory
    lcd_command(1 << LCD_CGRAM);
    for (uint8_t i = 0; i < 48; i++)
    {
        // Store all new chars to memory line by line
        lcd_data(customChar[i]);
    }
    // Set DDRAM address to display characters
    lcd_command(1 << LCD_DDRAM);

    lcd_gotoxy(1, 1); lcd_putc(4);lcd_puts("0        ");lcd_putc(5);lcd_puts("0");
    //lcd_gotoxy(7, 0); lcd_puts("a");  // Put ADC value in decimal
    //lcd_gotoxy(12,0); lcd_puts("b");  // Put ADC value in hexadecimal

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

    //set pins 2 and 3 on Arduino as tri-state input
    DDRD = DDRD & ~((1<<PD2)|(1<<PD3));
    PORTD = PORTD & ~((1<<PD2)|(1<<PD3));

    GPIO_mode_input_pullup(&DDRB, PB4);

    //set pins 2 and 3 to interrupt on any logical change
    MCUCR |= (1<<ISC00)|(1<<ISC10);
    MCUCR = MCUCR & ~((1<<ISC01)|(1<<ISC11));

    //enable interrupts on pins 2 and 3
    EIMSK |= (1<<INT0)|(1<<INT1);
    // Configure 16-bit Timer/Counter1 to start ADC conversion
    // Set prescaler to 33 ms and enable overflow interrupt
    TIM2_overflow_16ms();
    TIM2_overflow_interrupt_enable();
    TIM1_overflow_interrupt_enable();

    // Enables interrupts by setting the global interrupt mask
    sei();

    lcd_gotoxy(0,0);
    lcd_puts("                ");
    lcd_gotoxy(0,0);
    lcd_puts("GAME START? [OK]");
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
  static uint8_t OVF_count=0;
  OVF_count++;
    // Start ADC conversion
    if(cycle==0 && OVF_count>1){
      ADMUX = ADMUX & ~(1<<MUX3 | 1<<MUX2 | 1<< MUX1 | 1<< MUX0);
      ADCSRA |= (1<<ADSC);
      cycle=1;
      OVF_count=0;
    }
    else if(OVF_count>1){
      ADMUX = ADMUX & ~(1<<MUX3 | 1<<MUX2 | 1<< MUX1);
      ADMUX = ADMUX | (1<<MUX0);
      ADCSRA |= (1<<ADSC);
      cycle=0;
      OVF_count=0;
    }

    if(int_zero==1 && int_one==2){
      counter_CW();
    }
    if(int_zero==2 && int_one==1){
      CW();
    }

    if(GPIO_read(&PINB, PB4)==0 && button_mem==0)
    {
      button_press();
      button_mem=1;
    }
    else if(GPIO_read(&PINB, PB4)==1){
      button_mem=0;
    }
    if(game_status==1)
    {
      uint8_t iter=1;
      while(iter<11)
      {
        if(TCNT1>65536/10*iter)
        {
          lcd_gotoxy(4+iter,0);
          lcd_putc(255);
        }
        iter++;
      }
    }
}

/**********************************************************************
 * Function: ADC complete interrupt
 * Purpose:  Display converted value on LCD screen.
 **********************************************************************/
ISR(ADC_vect)
{
  uint16_t value;
  //char string_hex[3];
  // Read converted value
  // Note that, register pair ADCH and ADCL can be read as a 16-bit value ADC
  value = ADC;
  // Convert "value" to "string" and display it
  if (cycle==0){
    memory_y=value;
    //itoa(memory_y, string_hex, 10);
    //lcd_gotoxy(12, 1); lcd_puts("    "); 
    //lcd_gotoxy(12, 1); lcd_puts(string_hex);
    if(memory_y<200 && x_key_res==0 && y_key_res==0 && game_status==1) {y_key_res=1;move_check(0);}
    else if(memory_y>800 && x_key_res==0 && y_key_res==0 && game_status==1) {y_key_res=1;move_check(2);}
    else if(memory_y>400 && memory_y<600){y_key_res=0;}
  }
  else{
    memory_x=value;
    //itoa(memory_x, string_hex, 10);
    //lcd_gotoxy(7, 1); lcd_puts("    "); 
    //lcd_gotoxy(7, 1); lcd_puts(string_hex);
    if(memory_x<200 && x_key_res==0 && y_key_res==0 && game_status==1) {x_key_res=1;move_check(3);}
    else if(memory_x>800 && x_key_res==0 && y_key_res==0 && game_status==1) {x_key_res=1;move_check(1);}
    else if(memory_x>400 && memory_x<600) {x_key_res=0;}
  }
}

ISR(INT0_vect){
  if(int_zero==0){
    int_zero=int_one+1;
  }
}
ISR(INT1_vect){
  if(int_one==0){
    int_one=int_zero+1;
  }
}

ISR(TIMER1_OVF_vect)
{
  end_game();
}