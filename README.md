# FlipDot 5x7-Controller

Adafruit GFX based Library for 5x7 FlipDot breakout board which can be purchased at [HannIO](http://hannio.org/produkt/5x7-flipdot-controller/).

## Wiring

Note the silkscreen error on some PCBs.
Correct pinout is 12V|12V|GND|SDA|SCL|GND|LAT|5V|GND
For soldering instructions and further connection hints including daisychaining see [connection diagram](5x7FlipDotController2-5_Connection_Diagram.pdf).

### With Arduino on PCB (deprecated)

Solder the Arduino with the USB-Jack to the outside.
Use the Graphic-Test sketch to test your PCB, connect 12V.
For connecting more than one PCB, flash the Slave-Sketch to the other Arduinos.
Then jumper SDA, SCL, 12V and GND.

### With 595-ShiftRegisters (new version)

Flash ShiftRegister from examples to your Arduino, instructions are in sketch.