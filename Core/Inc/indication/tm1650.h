/*
 * tm1650.h
 *
 *  Created on: Feb 21, 2024
 *      Author: lepikhov
 */

#ifndef INC_INDICATION_TM1650_H_
#define INC_INDICATION_TM1650_H_

#define TM1650_DISPLAY_BASE 0x34 // Address of the left-most digit
#define TM1650_DCTRL_BASE   0x24 // Address of the control register of the left-most digit
#define TM1650_NUM_DIGITS   16 // max number of digits
#define TM1650_MAX_STRING   128 // number of digits
#define TM1650_TX_BUFFER_SIZE 128 // i2c tx buffer size

#define TM1650_BIT_ONOFF    0b00000001
#define TM1650_MSK_ONOFF    0b11111110
#define TM1650_BIT_DOT      0b00000001
#define TM1650_MSK_DOT      0b11110111
#define TM1650_DOT          0b10000000
#define TM1650_BRIGHT_SHIFT 4
#define TM1650_MSK_BRIGHT   0b10001111
#define TM1650_MIN_BRIGHT   0
#define TM1650_MAX_BRIGHT   7

#define _HI2C hi2c2
#define I2C_WRITE(_addr, _buffer, _size, _timeout) HAL_I2C_Master_Transmit(&_HI2C, _addr, _buffer, _size, _timeout)


#include <stdbool.h>


//class TM1650 {
//	public:
void TM1650_TM1650(unsigned int aNumDigits);
bool	TM1650_init();
void	TM1650_clear();
void	TM1650_displayOn();
void	TM1650_displayOff();
void	TM1650_displayState(bool aState);
void	TM1650_displayString(char *aString);
void	TM1650_displayChar(uint8_t pos, uint8_t data, bool dot);
void	TM1650_directLEDdrive(uint8_t pos, uint8_t aData);
int 	TM1650_displayRunning(char *aString);
int 	TM1650_displayRunningShift();
void	TM1650_setBrightness(unsigned int aValue);
void	TM1650_setBrightnessGradually(unsigned int aValue);
unsigned int TM1650_getBrightness();
void	TM1650_controlPosition(unsigned int aPos, uint8_t aValue);
void	TM1650_setPosition(unsigned int aPos, uint8_t aValue);
void	TM1650_setDot(unsigned int aPos, bool aState);
uint8_t	TM1650_getPosition(unsigned int aPos);
unsigned int	TM1650_getNumPositions();
uint8_t	TM1650_getButtons(void);

//	private:
struct TM1650_t {
		char 	*iPosition;
		bool	iActive;
		unsigned int	iNumDigits;
		unsigned int	iBrightness;
		char	iString[TM1650_MAX_STRING+1];
		uint8_t 	iBuffer[TM1650_NUM_DIGITS+1];
		uint8_t 	iCtrl[TM1650_NUM_DIGITS];
		uint8_t		txBuffer[TM1650_TX_BUFFER_SIZE];
};

#endif /* INC_INDICATION_TM1650_H_ */
