# FlipDot 5x7-Controller

Adafruit GFX Based for 5x7 FlipDot Breakout Board

## Wiring

Note the silkscreen error on some PCBs.
Correct pinout is 12V|12V|GND|SDA|SCL|GND|LAT|5V|GND

### With Arduino on PCB

Solder the Arduino with the USB-Jack to the outside.
Use the Graphic-Test sketch to test your PCB, connect 12V.
For connecting more than one PCB, flash the Slave-Sketch to the other Arduinos.
Then jumper SDA, SCL, 12V and GND.

### With 595-ShiftRegisters

Flash ShiftRegister from examples to your Arduino, instructions are in sketch.
