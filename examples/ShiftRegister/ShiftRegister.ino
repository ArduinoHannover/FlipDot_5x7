#include <FlipDot_5x7.h>
#define CLOCK 13
#define DATA  11
#define LATCH 10

/*
Wiring:

OUT  Not connected to Arduino
5V   5V
LAT  LATCH
GND  GND
SCL  CLOCK
SDA  DATA
GND  GND
12V  VIN
12V  VIN

Tie the OUT-pin from the module to SDA of the next module.
Daisychain the other pins.

NOTE:
There is a mistake (LAT, 5V) in the silkscreen on a batch of the PCBs.
Above wiring is correct.


Change false to true if the color is inverted.
*/
FlipDot_5x7 flipdot(1, 1, false);

void setup() {
	flipdot.begin(DATA, CLOCK, LATCH);
	flipdot.fillScreen(FLIPDOT_YELLOW);
	flipdot.display();
	delay(1000);
	flipdot.fillScreen(FLIPDOT_BLACK);
	flipdot.display();
	delay(1000);
}

void loop() {
	flipdot.setTextColor(FLIPDOT_YELLOW, FLIPDOT_BLACK);
	for (char c = '0'; c <= 'z'; c++) {
		flipdot.setCursor(0,0);
		flipdot.write(c);
		flipdot.display();
		delay(400);
	}
}
