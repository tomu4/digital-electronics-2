# Lab 1: Tomas Uchytil

### Morse code

1. Listing of C code which repeats one "dot" and one "comma" (BTW, in Morse code it is letter `A`) on a LED. Always use syntax highlighting, meaningful comments, and follow C guidelines:

```c
#define SHORT_DELAY 200 // Delay in milliseconds
#define LONG_DELAY 600 // Delay in milliseconds
#define PAUSE_DELAY 300 // Delay in milliseconds

int main(void)
{
    // Set pin where on-board LED is connected as output
    pinMode(LED_GREEN, OUTPUT);

    // Infinite loop
    while (1)
    {
        // Turn ON/OFF on-board LED
        digitalWrite(LED_GREEN, HIGH);

        // Pause several milliseconds
        _delay_ms(SHORT_DELAY);

        digitalWrite(LED_GREEN, LOW);

        _delay_ms(PAUSE_DELAY);

        digitalWrite(LED_GREEN, HIGH);

        _delay_ms(LONG_DELAY);

        digitalWrite(LED_GREEN, LOW);

        _delay_ms(PAUSE_DELAY);
        _delay_ms(PAUSE_DELAY);
    }

    // Will never reach this
    return 0;
}
```

2. Scheme of Morse code application, i.e. connection of AVR device, LED, resistor, and supply voltage. The image can be drawn on a computer or by hand. Always name all components and their values!

   ![scheme](images/schematic.png)