/*
 * tm1650.c
 *
 *  Created on: Feb 21, 2024
 *      Author: lepikhov
 */
#include <string.h>

#include "main.h"
#include "i2c.h"
#include "TM1650.h"


const uint8_t TM1650_CDigits[128] = {
		//0x00  0x01  0x02  0x03  0x04  0x05  0x06  0x07  0x08  0x09  0x0A  0x0B  0x0C  0x0D  0x0E  0x0F
		  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0x00
		  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0x10
		  0x00, 0x82, 0x21, 0x00, 0x00, 0x00, 0x00, 0x02, 0x39, 0x0F, 0x00, 0x00, 0x00, 0x40, 0x80, 0x00, // 0x20
		  0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7f, 0x6f, 0x00, 0x00, 0x00, 0x48, 0x00, 0x53, // 0x30
		  0x00, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71, 0x6F, 0x76, 0x06, 0x1E, 0x00, 0x38, 0x00, 0x54, 0x3F, // 0x40
		  0x73, 0x67, 0x50, 0x6D, 0x78, 0x3E, 0x00, 0x00, 0x00, 0x6E, 0x00, 0x39, 0x00, 0x0F, 0x00, 0x08, // 0x50
		  0x63, 0x5F, 0x7C, 0x58, 0x5E, 0x7B, 0x71, 0x6F, 0x74, 0x02, 0x1E, 0x00, 0x06, 0x00, 0x54, 0x5C, // 0x60
		  0x73, 0x67, 0x50, 0x6D, 0x78, 0x1C, 0x00, 0x00, 0x00, 0x6E, 0x00, 0x39, 0x30, 0x0F, 0x00, 0x00  // 0x70
};

struct TM1650_struct TM1650;

/** Constructor, uses default values for the parameters
 * so could be called with no parameters.
 * aNumDigits - number of display digits (default = 4)
 */
void TM1650_TM1650(unsigned int aNumDigits) {
	TM1650.iNumDigits =  (aNumDigits > TM1650_NUM_DIGITS) ? TM1650_NUM_DIGITS : aNumDigits;
}

/** Initialization
 * initializes the driver. Turns display on, but clears all digits.
 */
bool TM1650_init() {
	TM1650.iPosition = NULL;
	for (unsigned int i=0; i<TM1650.iNumDigits; i++) {
		TM1650.iBuffer[i] = 0;
		TM1650.iCtrl[i] = 0;
	}
	//Wire.beginTransmission(TM1650_DISPLAY_BASE);
	//TM1650.iActive = (Wire.endTransmission() == 0);
	TM1650.txBuffer[0] = TM1650_DISPLAY_BASE;
	uint32_t status = I2C_WRITE((TM1650_DISPLAY_BASE) << 1,TM1650.txBuffer,1,10);
	TM1650.iActive = (status == HAL_OK);
	TM1650_clear();
	TM1650_displayOn();
	return TM1650.iActive;
}

/** Set brightness of all digits equally
 * aValue - brightness value with 1 being the lowest, and 7 being the brightest
 */
void TM1650_setBrightness(unsigned int aValue) {
	if (!TM1650.iActive) return;

	TM1650.iBrightness = (aValue > TM1650_MAX_BRIGHT) ? TM1650_MAX_BRIGHT : aValue;

	for (unsigned int i=0; i<TM1650.iNumDigits; i++) {
		//Wire.beginTransmission(TM1650_DCTRL_BASE+(int)i);
		TM1650.iCtrl[i] = (TM1650.iCtrl[i] & TM1650_MSK_BRIGHT) | ( TM1650.iBrightness << TM1650_BRIGHT_SHIFT );
		//Wire.write((uint8_t) TM1650.iCtrl[i]);
		//Wire.endTransmission();
		//TM1650.txBuffer[0] = TM1650_DCTRL_BASE+(uint8_t)i;
		TM1650.txBuffer[0] = TM1650.iCtrl[i];
		I2C_WRITE((TM1650_DCTRL_BASE+(uint8_t)i) << 1,TM1650.txBuffer,1,10);
	}
}

/** Set brightness of all digits equally
 * aValue - brightness value with 1 being the lowest, and 7 being the brightest
 */
void TM1650_setBrightnessGradually(unsigned int aValue) {
	if (!TM1650.iActive || aValue == TM1650.iBrightness) return;
	if (aValue > TM1650_MAX_BRIGHT) aValue = TM1650_MAX_BRIGHT;
	int step = (aValue < TM1650.iBrightness) ? -1 : 1;
	unsigned int i = TM1650.iBrightness;
	do {
		TM1650_setBrightness(i);
		//delay(50);
		HAL_Delay(50);
		i += step;
	} while (i!=aValue);
}

/** Turns display on or off according to aState
 */
void TM1650_displayState(bool aState) {
	if (aState) TM1650_displayOn ();
	else TM1650_displayOff();
}

/** Turns the display on
 */
void TM1650_displayOn() {
	if (!TM1650.iActive) return;
	for (unsigned int i=0; i<TM1650.iNumDigits; i++) {
		//Wire.beginTransmission(TM1650_DCTRL_BASE+(int)i);
		TM1650.iCtrl[i] = (TM1650.iCtrl[i] & TM1650_MSK_ONOFF) | TM1650_BIT_DOT;
		//Wire.write((uint8_t) iCtrl[i]);
		//Wire.endTransmission();
		//TM1650.txBuffer[0] = TM1650_DCTRL_BASE+(uint8_t)i;
		TM1650.txBuffer[0] = TM1650.iCtrl[i];
		I2C_WRITE((TM1650_DCTRL_BASE+(uint8_t)i) << 1,TM1650.txBuffer,1,10);

	}
}

/** Turns the display off
 */
void TM1650_displayOff() {
	if (!TM1650.iActive) return;
	for (unsigned int i=0; i<TM1650.iNumDigits; i++) {
		//Wire.beginTransmission(TM1650_DCTRL_BASE+(int)i);
		TM1650.iCtrl[i] = (TM1650.iCtrl[i] & TM1650_MSK_ONOFF);
		//Wire.write((uint8_t) iCtrl[i]);
		//Wire.endTransmission();
		//TM1650.txBuffer[0] = TM1650_DCTRL_BASE+(uint8_t)i;
		TM1650.txBuffer[0] = TM1650.iCtrl[i];
		I2C_WRITE((TM1650_DCTRL_BASE+(uint8_t)i) << 1,TM1650.txBuffer,1,10);
	}
}

/** Directly write to the CONTROL register of the digital position
 * aPos = position to set the control register for
 * aValue = value to write to the position
 *
 * Internal control buffer is updated as well
 */
void TM1650_controlPosition(unsigned int aPos, uint8_t aValue) {
	if (!TM1650.iActive) return;
	if (aPos < TM1650.iNumDigits) {
		//Wire.beginTransmission(TM1650_DCTRL_BASE + (int)aPos);
		TM1650.iCtrl[aPos] = aValue;
		//Wire.write(aValue);
		//Wire.endTransmission();
	}
}

/** Directly write to the digit register of the digital position
 * aPos = position to set the digit register for
 * aValue = value to write to the position
 *
 * Internal position buffer is updated as well
 */
void TM1650_setPosition(unsigned int aPos, uint8_t aValue) {
	if (!TM1650.iActive) return;
	if (aPos < TM1650.iNumDigits) {
		//Wire.beginTransmission(TM1650_DISPLAY_BASE + (int)aPos);
		TM1650.iBuffer[aPos] = aValue;
		//Wire.write(aValue);
		//Wire.endTransmission();
	}
}

/** Directly set/clear a 'dot' next to a specific position
 * aPos = position to set/clear the dot for
 * aState = display the dot if true, clear if false
 *
 * Internal buffer is updated as well
 */
void	TM1650_setDot(unsigned int aPos, bool aState) {
	TM1650.iBuffer[aPos] = (TM1650.iBuffer[aPos] & 0x7F) | (aState ? TM1650_DOT : 0);
	TM1650_setPosition(aPos, TM1650.iBuffer[aPos]);
}

/** Clear all digits. Keep the display on.
 */
void TM1650_clear() {
	if (!TM1650.iActive) return;
	for (unsigned int i=0; i<TM1650.iNumDigits; i++) {
		//Wire.beginTransmission(TM1650_DISPLAY_BASE+(int)i);
		TM1650.iBuffer[i] = 0;
		//Wire.write((uint8_t) 0);
		//Wire.endTransmission();
		//TM1650.txBuffer[0] = TM1650_DISPLAY_BASE+(uint8_t)i;
		TM1650.txBuffer[0] = 0;
		I2C_WRITE((TM1650_DISPLAY_BASE+(uint8_t)i) << 1,TM1650.txBuffer,1,10);
	}
}

/** Display string on the display
 * aString = character array to be displayed
 *
 * Internal buffer is updated as well
 * Only first N positions of the string are displayed if
 *  the string is longer than the number of digits
 */
void TM1650_displayString(char *aString) {
	if (!TM1650.iActive) return;
	for (unsigned int i=0; i<TM1650.iNumDigits; i++) {
		uint8_t a = ((uint8_t) aString[i]) & ~TM1650_DOT;
		uint8_t dot = ((uint8_t) aString[i]) & TM1650_DOT;
		TM1650.iBuffer[i] = TM1650_CDigits[a];

		if (a) {
			//Wire.beginTransmission(TM1650_DISPLAY_BASE+(int)i);
			//Wire.write(iBuffer[i] | dot);
			//Wire.endTransmission();
			//TM1650.txBuffer[0] = TM1650_DISPLAY_BASE+(uint8_t)i;
			TM1650.txBuffer[0] = TM1650.iBuffer[i] | dot;
			I2C_WRITE((TM1650_DISPLAY_BASE+(uint8_t)i) << 1,TM1650.txBuffer,1,10);
		}
		else break;
	}
}

/** Display string on the display in a running fashion
 * aString = character array to be displayed
 *
 * Starts with first N positions of the string.
 * Subsequent characters are displayed with 1 char shift each time displayRunningShift() is called
 *
 * returns: number of iterations remaining to display the whole string
 */
int TM1650_displayRunning(char *aString) {
	strncpy(TM1650.iString, aString, TM1650_MAX_STRING+1);
	TM1650.iPosition = TM1650.iString;
	TM1650.iString[TM1650_MAX_STRING] = '\0'; //just in case.
	TM1650_displayString(TM1650.iPosition);
	unsigned int l = strlen(TM1650.iPosition);
	if (l <= TM1650.iNumDigits) return 0;
	return (l - TM1650.iNumDigits);
}

/** Display next segment (shifting to the left) of the string set by displayRunning()
 * Starts with first N positions of the string.
 * Subsequent characters are displayed with 1 char shift each time displayRunningShift is called
 *
 * returns: number of iterations remaining to display the whole string
 */
int TM1650_displayRunningShift() {
	if (strlen(TM1650.iPosition) <= TM1650.iNumDigits) return 0;
	TM1650_displayString(++TM1650.iPosition);
	return (strlen(TM1650.iPosition) - TM1650.iNumDigits);
}

/** Display character on the display
 * aPos = the position of the display character
 * aData = character to be displayed
 * aDot = display the dot if true, clear if false
 *
 * Display of the character at the specified position of the display.
 */
void TM1650_displayChar(uint8_t aPos, uint8_t aData, bool aDot) {
	if (!TM1650.iActive) return;
	uint8_t a = aData & ~TM1650_DOT;
	aData = TM1650_CDigits[a];

	if (aDot) aData |= TM1650_DOT;
	TM1650.iBuffer[aPos] = aData;
	//Wire.beginTransmission(TM1650_DISPLAY_BASE + aPos);
	//Wire.write(aData);
	//Wire.endTransmission();
	TM1650.txBuffer[0] = aData;
	I2C_WRITE((TM1650_DISPLAY_BASE+(uint8_t)aPos) << 1,TM1650.txBuffer,1,10);
}

void	TM1650_directLEDdrive(uint8_t aPos, uint8_t aData) {
	if (!TM1650.iActive) return;

	TM1650.iBuffer[aPos] = aData;
	//Wire.beginTransmission(TM1650_DISPLAY_BASE + aPos);
	//Wire.write(aData);
	//Wire.endTransmission();
	TM1650.txBuffer[0] = aData;
	I2C_WRITE((TM1650_DISPLAY_BASE+(uint8_t)aPos) << 1,TM1650.txBuffer,1,10);
}

/** Getting the buttons pressed
 *
 * returns: the code of the pressed buttons, for details refer to the documentation on the chip TM1650
 */
uint8_t TM1650_getButtons(void) {
	uint8_t keys = 0;
	//Wire.requestFrom(TM1650_DCTRL_BASE, 2);
	//keys = Wire.read();
	//Wire.read();
	return keys;
}

unsigned int TM1650_getBrightness() { return TM1650.iBrightness; }
uint8_t	TM1650_getPosition(unsigned int aPos) { return TM1650.iBuffer[aPos]; }
unsigned int	TM1650_getNumPositions() { return TM1650.iNumDigits; }
