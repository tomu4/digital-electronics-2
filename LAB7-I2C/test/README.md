# Lab 7: Tomas Uchytil

### Arduino Uno pinout

1. In the picture of the Arduino Uno board, mark the pins that can be used for the following functions/operations:
   * PWM generators from Timer0, Timer1, Timer2
   * analog channels for ADC
   * UART pins
   * I2C pins
   * SPI pins
   * external interrupt pins INT0, INT1

   ![your figure](img/UNO_pinout.jpg)

### I2C communication

2. Draw a timing diagram of I2C signals when calling function `rtc_read_years()`. Let this function reads one byte-value from RTC DS3231 address `06h` (see RTC datasheet) in the range `00` to `99`. Specify when the SDA line is controlled by the Master device and when by the Slave device. Draw the whole request/receive process, from Start to Stop condition. The image can be drawn on a computer (by [WaveDrom](https://wavedrom.com/) for example) or by hand. Name all parts of timing diagram.
```wavedrom
{signal: [
  {name: 'data', wave: '1.4.5.............7.8.5...............8.14.5.............7.8.5...............6.4.1', data: "start 0x68 write ACK 0x06 ACK rep.start 0x68 read ACK 0x22 NACK end", phase: 0.5},
  {name: 'SCL', wave: '1..01010101010101010101010101010101010101101010101010101010101010101010101010101..'},
  {name: 'SDA', wave: '1..01.1.0.1.0.0.0.0.0.0.0.0.0.0.1.1.0.0.1.01.1.0.1.0.0.0.1.0.0.0.1.0.0.0.1.0.1.01.',phase: 0.5},
]}
```
   ![I2C waveform](img/I2C_year.jpg)

### Meteo station

Consider an application for temperature and humidity measurements. Use sensor DHT12, real time clock DS3231, LCD, and one LED. Every minute, the temperature, humidity, and time is requested from Slave devices and values are displayed on LCD screen. When the temperature is above the threshold, turn on the LED.

3. Draw a flowchart of `TIMER1_OVF_vect` (which overflows every 1&nbsp;sec) for such Meteo station. The image can be drawn on a computer or by hand. Use clear description of individual algorithm steps.

   ![flowchart](img/temp_hum_time_diagram.jpg)