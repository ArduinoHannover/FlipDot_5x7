#include "FlipDot_5x7.h"

#ifndef ESP8266
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
#endif //ESP8266



FlipDot_5x7::FlipDot_5x7(uint8_t xModules, uint8_t yModules, boolean invert) :
	// +2 is a dirty hack to allow displaying chars on last display.
	Adafruit_GFX(xModules*FLIPDOT_MODULE_WIDTH+2, yModules*FLIPDOT_MODULE_HEIGHT),
	_xModules(xModules),
	_yModules(yModules),
	_invert(invert) {
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
#warning "Alternatively use with begin(data, clock, latch)"
#warning "Do not use as Slave, as no IO-control is supported."
#else
	pinMode(_dataR, OUTPUT);
	digitalWrite(_dataR, LOW);  
	pinMode(_dataC, OUTPUT);
	digitalWrite(_dataC, LOW);  
	pinMode(_en, OUTPUT);
	digitalWrite(_en, LOW);
	pinMode(_rowA, OUTPUT);
	digitalWrite(_rowA, LOW);  
	pinMode(_rowB, OUTPUT);
	digitalWrite(_rowB, LOW);  
	pinMode(_rowC, OUTPUT);
	digitalWrite(_rowC, LOW);  
	pinMode(_colA, OUTPUT);
	digitalWrite(_colA, LOW);  
	pinMode(_colB, OUTPUT);
	digitalWrite(_colB, LOW);  
	pinMode(_colC, OUTPUT);
	digitalWrite(_colC, LOW);
	pinMode(_demo, INPUT_PULLUP);
#endif //ESP8266
	fillScreen(FLIPDOT_YELLOW);
	display();
	fillScreen(FLIPDOT_BLACK);
	display();
	setTextColor(FLIPDOT_YELLOW, FLIPDOT_BLACK);
}

void FlipDot_5x7::begin(uint8_t data, uint8_t clock, uint8_t latch) {
	_data = data;
	_clock = clock;
	_latch = latch;
	digitalWrite(_latch, LOW);
	pinMode(_data, OUTPUT);
	pinMode(_clock, OUTPUT);
	pinMode(_latch, OUTPUT);
	_serialMode = true;
	fillScreen(FLIPDOT_YELLOW);
	display();
	fillScreen(FLIPDOT_BLACK);
	display();
	setTextColor(FLIPDOT_YELLOW, FLIPDOT_BLACK);
}

#ifndef ESP8266
void FlipDot_5x7::selectRow(uint8_t row) {
	digitalWrite(_rowA, row & 1);
	digitalWrite(_rowB, row & 2);
	digitalWrite(_rowC, row & 4);
}

void FlipDot_5x7::selectCol(uint8_t col) {
	digitalWrite(_colA, col & 1);
	digitalWrite(_colB, col & 2);
	digitalWrite(_colC, col & 4);
}
#endif //ESP8266

void FlipDot_5x7::displayPixel(int16_t x, int16_t y, boolean color) {
	if (x < FLIPDOT_MODULE_WIDTH && y < FLIPDOT_MODULE_HEIGHT) {
		if (_serialMode) {
			// DR R0 R1 R2 DC C0 C1 C2
			uint8_t data = color != _invert;
			data |= y << 1;
			data |= (color == _invert) << 4;
			data |=  x << 5;
			shiftOut(_data, _clock, LSBFIRST, data);
		} else {
#ifndef ESP8266
			selectCol(x);
			selectRow(y);
			digitalWrite(_dataC, color == _invert);
			digitalWrite(_dataR, color != _invert);
			digitalWrite(_en, HIGH);
			delayMicroseconds(FLIPDOT_PULSE_DURATION);
			digitalWrite(_en, LOW);
			delay(_pixelDelay);
#endif
		}
	}
}

void FlipDot_5x7::drawPixel(int16_t x, int16_t y, uint16_t color) {
	if (x >= width() || y >= height()) return;
	imageBuffer[y*width()+x] = (color>0);
}

void FlipDot_5x7::setPixelDelay(uint8_t pixelDelay) {
	_pixelDelay = pixelDelay;
}

void FlipDot_5x7::display(void) {
	if (_serialMode) {
		for (uint8_t x = 0; x < FLIPDOT_MODULE_WIDTH; x++) {
			for (uint8_t y = 0; y < FLIPDOT_MODULE_HEIGHT; y++) {
				//uint8_t dnc = 0;
				for (int8_t yModule = _yModules-1; yModule > -1; yModule--) {
					for (int8_t xModule = (yModule%2)?(_xModules-1):0;
						((yModule%2)?-1:xModule) < ((yModule%2)?xModule:_xModules);
						xModule += (yModule%2)?-1:1) {
						uint8_t _x = xModule*FLIPDOT_MODULE_WIDTH+x;
						uint8_t _y = yModule*FLIPDOT_MODULE_HEIGHT+y;
						//if (oldImageBuffer[y*width()+x] == imageBuffer[y*width()+x]) dnc++;
						displayPixel(x,y,imageBuffer[_y*width()+_x]);
						oldImageBuffer[_y*width()+_x] = imageBuffer[_y*width()+_x];
					}
				}
				//if (dnc == _xModules*_yModules) continue;
				digitalWrite(_latch, HIGH);
				delayMicroseconds(FLIPDOT_PULSE_DURATION);
				digitalWrite(_latch, LOW);
				delay(_pixelDelay);
			}
		}
		//disable
		for (uint8_t m = 0; m < _xModules*_yModules; m++) {
			shiftOut(_data, _clock, MSBFIRST, 0);
		}
		digitalWrite(_latch, HIGH);
		digitalWrite(_latch, LOW);
	} else {
		for (uint8_t xModule = 0; xModule < _xModules; xModule++) {
			for (uint8_t yModule = 0; yModule < _yModules; yModule++) {
#ifndef ESP8266
				if (xModule == 0 && yModule == 0) { //Master module or this is a slave
					for (uint8_t x = 0; x < FLIPDOT_MODULE_WIDTH; x++) {
						for (uint8_t y = 0; y < FLIPDOT_MODULE_HEIGHT; y++) {
							if (oldImageBuffer[y*width()+x] == imageBuffer[y*width()+x]) continue;
							displayPixel(x,y,imageBuffer[y*width()+x]);
							oldImageBuffer[y*width()+x] = imageBuffer[y*width()+x];
						}
					}
				} else { //Slaves
					Wire.beginTransmission(FLIPDOT_I2C_OFFSET + yModule * _xModules + xModule - 1);
#else
					Wire.beginTransmission(FLIPDOT_I2C_OFFSET + yModule * _xModules + xModule);
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
}