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
FlipDot_5x7 flipdot(2, 1, false); //two displays daisychained

char* text = "Hello World  ";

void setup() {
	Serial.begin(9600);
	flipdot.begin(DATA, CLOCK, LATCH);
	flipdot.setTextWrap(false);
}

void loop() {
	flipdot.setTextColor(FLIPDOT_YELLOW, FLIPDOT_BLACK);
	for (int16_t i = flipdot.width(); i > (int16_t)(strlen(text))*-6; i--) {
		flipdot.setCursor(i,0);
		flipdot.print(text);
		flipdot.display();
		delay(50);
	}
}
