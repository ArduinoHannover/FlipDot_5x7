#include <FlipDot_5x7.h>

FlipDot_5x7_Slave flipdot(false);

void setup() {
	flipdot.begin();
	Wire.onReceive(receiveEvent);
}

void receiveEvent(int n) {
	flipdot.receiveEvent(n);
}

void loop() {
}