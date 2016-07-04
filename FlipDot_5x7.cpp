#include "FlipDot_5x7.h"

//FlipDot_5x7_Slave::FlipDot_5x7_Slave(boolean invert) : FlipDot_5x7(1,1,invert);

void FlipDot_5x7_Slave::begin() {
	pinMode(addr1, INPUT_PULLUP);
	pinMode(addr2, INPUT_PULLUP);
	pinMode(addr3, INPUT_PULLUP);
	pinMode(addr4, INPUT_PULLUP);
	byte addr =
		((!digitalRead(addr4)) << 3) |
		((!digitalRead(addr3)) << 2) |
		((!digitalRead(addr2)) << 1) |
		!digitalRead(addr1);
	Wire.begin(addr + FLIPDOT_I2C_OFFSET);
	FlipDot_5x7::begin();
	setCursor(0,0);
	write(addr < 10 ? '0' + addr : 'A' - 10 + addr);
	display();
	delay(1500);
	fillScreen(FLIPDOT_BLACK);
	display();
	/*need to add that:
	setup() {	...	Wire.onReceive(receiveEvent);	...}
	void receiveEvent(int n) {flipdot.receiveEvent(n);}
	Or simply use the offered Slave.ino
	*/
}

void FlipDot_5x7_Slave::receiveEvent(int n) {
	int x=0;
	while(Wire.available()){
		byte rcv = Wire.read();
		for (uint8_t y = 0; y < FLIPDOT_MODULE_HEIGHT; y++) {
			drawPixel(x, y, (rcv>>y) & 1);
		}
		x = (x+1)%FLIPDOT_MODULE_WIDTH;
	}
	display();
}

FlipDot_5x7::FlipDot_5x7(uint8_t modulesX, uint8_t modulesY, boolean invert) :
	// +2 is a dirty hack to allow displaying chars on last display.
	Adafruit_GFX(modulesX*FLIPDOT_MODULE_WIDTH+2, modulesY*FLIPDOT_MODULE_HEIGHT),
	xModules(modulesX),
	yModules(modulesY) {
	this->invert = invert;
	imageBuffer = (boolean*) calloc(width()*height(), sizeof(boolean));
	oldImageBuffer = (boolean*) calloc(FLIPDOT_MODULE_WIDTH*height(), sizeof(boolean));
}

int16_t FlipDot_5x7::width() const {
	return _width-2;
}

void FlipDot_5x7::begin() {
	Wire.begin();
	#ifdef ESP8266
	#warning "Using ESP8266 as Master, connect I2C on IO4 (SDA) and IO5 (SCL)"
	#else
	pinMode(dataR, OUTPUT);
	digitalWrite(dataR, LOW);  
	pinMode(dataC, OUTPUT);
	digitalWrite(dataC, LOW);  
	pinMode(en, OUTPUT);
	digitalWrite(en, LOW);
	pinMode(rowA, OUTPUT);
	digitalWrite(rowA, LOW);  
	pinMode(rowB, OUTPUT);
	digitalWrite(rowB, LOW);  
	pinMode(rowC, OUTPUT);
	digitalWrite(rowC, LOW);  
	pinMode(colA, OUTPUT);
	digitalWrite(colA, LOW);  
	pinMode(colB, OUTPUT);
	digitalWrite(colB, LOW);  
	pinMode(colC, OUTPUT);
	digitalWrite(colC, LOW);
	pinMode(demo, INPUT_PULLUP);
	#endif
	fillScreen(FLIPDOT_YELLOW);
	display();
	fillScreen(FLIPDOT_BLACK);
	display();
	setTextColor(FLIPDOT_YELLOW, FLIPDOT_BLACK);
}

void FlipDot_5x7::selectRow(uint8_t row) {
	digitalWrite(rowA, row & 1);
	digitalWrite(rowB, row & 2);
	digitalWrite(rowC, row & 4);
}

void FlipDot_5x7::selectCol(uint8_t col) {
	digitalWrite(colA, col & 1);
	digitalWrite(colB, col & 2);
	digitalWrite(colC, col & 4);
}

void FlipDot_5x7::displayPixel(int16_t x, int16_t y, boolean color) {
	if (x < FLIPDOT_MODULE_WIDTH && y < FLIPDOT_MODULE_HEIGHT) {
		selectCol(x);
		selectRow(y);
		digitalWrite(dataC, color == invert);  
		digitalWrite(dataR, color != invert); 
		digitalWrite(en, HIGH);  
		delayMicroseconds(FLIPDOT_PULSE_DURATION);  
		digitalWrite(en, LOW);  
	}
}

void FlipDot_5x7::drawPixel(int16_t x, int16_t y, uint16_t color) {
	if (x >= width() || y >= height()) return;
	imageBuffer[y*width()+x] = (color>0);
}

void FlipDot_5x7::display(void) {
	for (uint8_t xModule = 0; xModule < xModules; xModule++) {
		for (uint8_t yModule = 0; yModule < yModules; yModule++) {
#ifndef ESP8266
			if (xModule == 0 && yModule == 0) { //Master module
				for (uint8_t x = 0; x < FLIPDOT_MODULE_WIDTH; x++) {
					for (uint8_t y = 0; y < FLIPDOT_MODULE_HEIGHT; y++) {
						if (oldImageBuffer[y*width()+x] == imageBuffer[y*width()+x]) continue;
						displayPixel(x,y,imageBuffer[y*width()+x]);
						oldImageBuffer[y*width()+x] = imageBuffer[y*width()+x];
					}
				}
			} else {
				Wire.beginTransmission(FLIPDOT_I2C_OFFSET + yModule * xModules + xModule - 1);
#else
				Wire.beginTransmission(FLIPDOT_I2C_OFFSET + yModule * xModules + xModule);
#endif //ESP8266
				for (uint8_t x = xModule*FLIPDOT_MODULE_WIDTH; x < (xModule+1)*FLIPDOT_MODULE_WIDTH; x++) {
					uint8_t wrt = 0;
					for (uint8_t y = yModule*FLIPDOT_MODULE_HEIGHT; y < (yModule+1)*FLIPDOT_MODULE_HEIGHT; y++) {
						wrt |= imageBuffer[y*width()+x] << (y%FLIPDOT_MODULE_HEIGHT);
					}
					Wire.write(wrt);
				}
				Wire.endTransmission();
#ifndef ESP8266
			}
#endif
		}
	}
}