#ifndef FLIPDOT_5X7_H
#define FLIPDOT_5X7_H

#include <Wire.h>
#include <Adafruit_GFX.h>

#define FLIPDOT_BLACK          0
#define FLIPDOT_YELLOW         1

#define FLIPDOT_PULSE_DURATION 500
#define FLIPDOT_I2C_OFFSET     8

#define FLIPDOT_MODULE_WIDTH   5
#define FLIPDOT_MODULE_HEIGHT  7

class FlipDot_5x7 : public Adafruit_GFX {
	private:
#ifndef ESP8266
		const uint8_t
			_dataR	= 10,
			_dataC	=  5,
			_en		=  6,
			_rowA	=  9,
			_rowB	=  8,
			_rowC	=  7,
			_colA	=  4,
			_colB	=  3,
			_colC	=  2,
			_demo	= A6;
#endif //ESP8266
		uint8_t
			_clock,
			_data,
			_latch;
		
		boolean *imageBuffer = NULL;
		boolean *oldImageBuffer = NULL;
		boolean _invert;
		boolean _serialMode = false;
		uint8_t _xModules, _yModules,
			_pixelDelay = 0;
		
#ifndef ESP8266
		void selectCol(uint8_t);
		void selectRow(uint8_t);
#endif //ESP8266
		void displayPixel(int16_t, int16_t, boolean);

	public:
		void begin(void);
		void begin(uint8_t, uint8_t, uint8_t);
		void drawPixel(int16_t, int16_t, uint16_t);
		void display(void);
		void setPixelDelay(uint8_t);
		int16_t width(void) const;
		FlipDot_5x7(uint8_t, uint8_t, boolean);
};

#ifndef ESP8266
class FlipDot_5x7_Slave : public FlipDot_5x7 {
	private:
		const uint8_t
			addr1	= A0,
			addr2	= A1,
			addr3	= A2,
			addr4	= A3;
	public:
		FlipDot_5x7_Slave(boolean invert) : FlipDot_5x7(1,1,invert) {}
		void begin(void);
		void receiveEvent(int);
};
#endif //ESP8266

#endif //FLIPDOT_5X7_H