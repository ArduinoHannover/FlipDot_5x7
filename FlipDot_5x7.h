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
		const uint8_t
			dataR	= 10,
			dataC	=  5,
			en		=  6,
			rowA	=  9,
			rowB	=  8,
			rowC	=  7,
			colA	=  4,
			colB	=  3,
			colC	=  2,
			demo	= A6;
		
		boolean *imageBuffer = NULL;
		boolean *oldImageBuffer = NULL;
		boolean invert;
		uint8_t xModules, yModules;
		
		void selectCol(uint8_t);
		void selectRow(uint8_t);
		void displayPixel(int16_t, int16_t, boolean);

	public:
		void begin(void);
		void drawPixel(int16_t, int16_t, uint16_t);
		void display(void);
		int16_t width(void) const;
		FlipDot_5x7(uint8_t, uint8_t, boolean);
};

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

#endif //FLIPDOT_5X7_H