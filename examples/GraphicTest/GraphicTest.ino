#include <FlipDot_5x7.h>

FlipDot_5x7 flipdot(1, 1, false);

void setup() {
	flipdot.begin();
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