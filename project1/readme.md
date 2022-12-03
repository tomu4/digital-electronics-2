# Project 1

Application of analog joy-stick (2 ADC channels, 1 push button), rotary encoder, and Digilent PmodCLP LCD module.

## Team members

* Tomas Uchytil (responsible for schematic, wiring, code, video and documentation)

## Hardware description

Insert descriptive text and schematic(s) of your implementation.

## Software description
Timer 2 (continuous, takes care of inputs):
![timer2 flowchart](img/timer2.jpeg)
Other interrupts:
![other interrupts](img/interrupts.jpeg)

Functions - part 1:
![functions part 1](img/functions%20-%20Page%201.jpeg)
Functions - part 2:
![functions part 2](img/functions%20-%20Page%202.jpeg)

There is only one source file where all the programming magic happens, and it's the [main.c](src/main.c) file. However, game program contains several functions simplifying both program orientation and programming experience. 

I'll start this documentation by explaining what each function does, then I'll tell something about how the app usually runs.

### function descriptions 

Put flowchats of your algorithm(s). Write descriptive text of your libraries and source files. Put direct links to these files in `src` or `lib` folders.

## Video

[![video](https://img.youtube.com/vi/JNuxtKqhXKQ/0.jpg)](https://youtu.be/JNuxtKqhXKQ)

## References

1. [Atmega 328P datasheet](https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf)
2. google images for pinout and connection examples