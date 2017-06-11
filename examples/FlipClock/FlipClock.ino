#include <FlipDot_5x7.h>
#include <Wire.h>
#include <TimeLib.h>
#include <DS3232RTC.h>

#define CLOCK 13
#define DATA  11
#define LATCH 12


//Uncomment your number of panels
//#define PANELS 3 //ultra narrow version
#define PANELS 4 //small version
//#define PANELS 5 //medium version
//#define PANELS 6 //wide version

//Uncomment if you like a blinking colon, every x seconds
//#define BLINK_DOT 1
//#define BLINK_DOT 2
//#define BLINK_DOT 5

/*
You'll need a compatible RTC (DS1307, DS3231, DS3232...) and the library from https://github.com/JChristensen/DS3232RTC
First set the time on the RTC without DST, then upload this sketch

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

FlipDot_5x7 flipdot(PANELS, 1, false);

#if PANELS < 3
#error "There must be at least 3 panels to display the time."
#endif //PANELS < 3

#if PANELS < 5 //Narrow font
uint8_t numbers[][3] = {
	{
		0b0111110,
		0b1000001,
		0b0111110,
	},
	{
		0b0000100,
		0b0000010,
		0b1111111,
	},
	{
		0b1110010,
		0b1001001,
		0b1000110,
	},
	{
		0b1000001,
		0b1001001,
		0b0110110,
	},
	{
		0b0001100,
		0b0001010,
		0b1111111,
	},
	{
		0b0101111,
		0b1001001,
		0b0110001,
	},
	{
		0b0111110,
		0b1001001,
		0b0110010,
	},
	{
		0b1100011,
		0b0011001,
		0b0000111,
	},
	{
		0b0110110,
		0b1001001,
		0b0110110,
	},
	{
		0b0100110,
		0b1001001,
		0b0111110,
	}
};
#endif //PANELS==4

boolean dot = true;
uint8_t disp_minute = 60;

void setup() {
	flipdot.begin(DATA, CLOCK, LATCH);
	flipdot.setTextWrap(false);
	setSyncProvider(RTC.get);
}

#if PANELS < 5
void showNumber(uint8_t n, uint8_t x) {
	if (n > 9) return;
	for (uint8_t x_ = 0; x_ < 3; x_++) {
		for (uint8_t y = 0; y < 7; y++) {
			flipdot.drawPixel(x + x_, y, numbers[n][x_] & (1 << y));
		}
	}
}
#endif //PANELS < 5

void loop() {
	time_t t = now();
	dst(t);
#ifdef BLINK_DOT
	if (minute(t) != disp_minute || (second(t) / BLINK_DOT) % 2 != dot) {
		dot = !dot;
#else
	if (minute(t) != disp_minute) {
#endif //BLINK_DOT
		flipdot.fillScreen(0);
#if PANELS < 5
		showNumber(hour(t) / 10, 0);
		showNumber(hour(t) % 10, 4);
	#if PANELS == 4
		flipdot.drawRect(9, 1, 2, 2, dot);
		flipdot.drawRect(9, 4, 2, 2, dot);
		showNumber(minute(t) / 10, 13);
		showNumber(minute(t) % 10, 17);
	#else //PANELS == 3
		showNumber(minute(t) / 10, 8);
		showNumber(minute(t) % 10, 12);
	#endif //PANELS == 4 / 3
#else //PANELS >= 5
		flipdot.setTextColor(1, 0);
		flipdot.setCursor(0, 0);
		flipdot.write('0' + (hour(t) / 10));
		flipdot.write('0' + (hour(t) % 10));
	#if PANELS == 5
		flipdot.drawPixel(12, 2, dot);
		flipdot.drawPixel(12, 4, dot);
		flipdot.setCursor(14, 0);
	#else //PANELS > 5
		flipdot.write(32 + dot * 26);
	#endif //PANELS == 5
		flipdot.write('0' + (minute(t) / 10));
		flipdot.write('0' + (minute(t) % 10));
#endif //PANELS < / => 5
		disp_minute = minute(t);
		flipdot.display();
	}
}

#define HOUR (60 * 60)
/*
 * Applies DST to time_t struct
 */
boolean dst(time_t &t) {
	if (month(t) > 3 && month(t) < 10) { //Apr-Sep
		t += HOUR;
		return true;
	} else if (month(t) == 3 && day(t) - weekday(t) >= 24) { //Date at or after last sunday in March
		if (weekday(t) == 1) { //Sunday to switch to dst
			if (hour(t) >= 2) { //Time after 2AM
				t += HOUR;
	  			return true;
			}
		} else { //Date after last sunday in March
			t += HOUR;
			return true;
		}
	} else if (month(t) == 10 && day(t) - weekday(t) < 24) { //Date before last sunday in October
		t += HOUR;
		return true;
	} else if (month(t) == 10 && day(t) - weekday(t) >= 24) { //Date at or after last sunday in March
		if (weekday(t) == 1) { //Sunday to switch back from dst
			if (hour(t) < 3) { //Time before 2AM without DST (3AM DST, wich doesn't exist)
				t += HOUR;
				return true;
			}
		}
	}
	return false;
}
