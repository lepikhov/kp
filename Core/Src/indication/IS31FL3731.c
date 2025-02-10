/*
 * IS31FL3731.c
 *
 *  Created on: Nov 18, 2024
 *      Author: lepikhov
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "main.h"
#include "i2c.h"

#include <indication/IS31FL3731.h>



uint8_t IS31FL3731_rows[11] = {0x00, 0x02, 0x04, 0x06, 0x08, 0x0A, 0x01, 0x03, 0x05, 0x07, 0x09};

uint8_t IS31FL3731_transfer_buffer[2];

bool IS31FL3731_write_reg(I2C_HandleTypeDef* handler, uint8_t reg, uint8_t data) {
	IS31FL3731_transfer_buffer[0] = reg;
	IS31FL3731_transfer_buffer[1] = data;

	return HAL_I2C_Master_Transmit(handler, (ADDR << 1), IS31FL3731_transfer_buffer, 2, ISSI_TIMEOUT)
			==HAL_OK;

}

bool IS31FL3731_write_PWM_reg(I2C_HandleTypeDef* handler, uint8_t start_reg, uint8_t data_buffer[8]){
	return HAL_I2C_Mem_Write(handler, (ADDR << 1), start_reg, I2C_MEMADD_SIZE_8BIT, data_buffer, 8, 1000)
			==HAL_OK;
}

bool IS31FL3731_init(I2C_HandleTypeDef* handler){
	// Select Function Reg
	if (!IS31FL3731_write_reg(handler, COMMAND_REGISTER, BANK_FUNCTIONREG)) return false;

	// Shutdown Screen
	if (!IS31FL3731_write_reg(handler, SHUTDOWN, 0x00)) return false;

	// Picture Mode
	if (!IS31FL3731_write_reg(handler, MODE_SELECT, 0x00)) return false;

	// Set Fade in / out times
	if (!IS31FL3731_write_reg(handler, BREATH_CONTROL_1, (FADE_OUT_TIME << 4) + FADE_IN_TIME)) return false;

	// Enable Breath and set Extinguish time
	if (!IS31FL3731_write_reg(handler, BREATH_CONTROL_2, (1 << 4) + EXTINGUISH_TIME)) return false;

	// Disp frame 0
	if (!IS31FL3731_write_reg(handler, FRAME_SELECT, 0x00)) return false;

	// Shut down audio sync
	if (!IS31FL3731_write_reg(handler, AUDIO_SYNC, 0x00)) return false;

	// Enable Blink
	if (!IS31FL3731_write_reg(handler, DISPLAY_OPTION, BLINK_PERIOD + 0x08)) return false; // + 0x08 to turn on

	// Select bank 1
	if (!IS31FL3731_write_reg(handler, COMMAND_REGISTER, BANK_1)) return false;

	// Turn on all LEDs
	for (uint8_t i = 0; i <= 10; i++) {
		if (!IS31FL3731_write_reg(handler, IS31FL3731_rows[i], 0xff)) return false;
	}

	// Turn off all Blinking
	for (uint8_t i = 0; i <= 10; i++) {
		if (!IS31FL3731_write_reg(handler, IS31FL3731_rows[i] + 0x12, 0x00)) return false;
	}

	// Set all PWM to INIT_PWM
	uint8_t data_buffer[8] = {INIT_PWM, INIT_PWM,
			INIT_PWM, INIT_PWM, INIT_PWM, INIT_PWM, INIT_PWM, INIT_PWM};

	for (uint8_t i = 0; i <= 10; i++) {
		if (!IS31FL3731_write_PWM_reg(handler, 0x24 + IS31FL3731_rows[i] * 0x08, data_buffer)) return false;
	}

	// Select Function Reg
	if (!IS31FL3731_write_reg(handler, COMMAND_REGISTER, BANK_FUNCTIONREG)) return false;

	// Disable Shutdown
	if (!IS31FL3731_write_reg(handler, SHUTDOWN, 0x01)) return false;

	return true;
}

bool IS31FL3731_send_buffer(I2C_HandleTypeDef* handler, uint8_t frameBuffer[11]) {
	// Select bank 1
	if (!IS31FL3731_write_reg(handler, COMMAND_REGISTER, BANK_1)) return false;

	// Send buffer
	for (uint8_t i = 0; i <= 10; i++) {
		if (!IS31FL3731_write_reg(handler, IS31FL3731_rows[i], frameBuffer[i])) return false;
	}

	return true;
}

bool IS31FL3731_send_buffer_PWM(I2C_HandleTypeDef* handler, uint8_t PWMBuffer[11][8]){
	// Select bank 1
	if (!IS31FL3731_write_reg(handler, COMMAND_REGISTER, BANK_1)) return false;

	// Send buffer for a whole row
	for (uint8_t i = 0; i <= 10; i++) {
		if (!IS31FL3731_write_PWM_reg(handler, 0x24 + IS31FL3731_rows[i] * 0x08, PWMBuffer[i])) return false;
	}

	return true;
}

bool IS31FL3731_send_buffer_BLINK(I2C_HandleTypeDef* handler, uint8_t blinkBuffer[11]){
	// Select bank 1
	if (!IS31FL3731_write_reg(handler, COMMAND_REGISTER, BANK_1)) return false;

	// Send buffer
	for (uint8_t i = 0; i <= 10; i++) {
		if (!IS31FL3731_write_reg(handler, IS31FL3731_rows[i] + 0x12, blinkBuffer[i])) return false;
	}

	return true;
}

bool IS31FL3731_clear_screen(I2C_HandleTypeDef* handler) {
	// Select bank 1
	if (!IS31FL3731_write_reg(handler, COMMAND_REGISTER, BANK_1)) return false;

	// Turn off all LEDs
	for (uint8_t i = 0; i <= 10; i++) {
		if (!IS31FL3731_write_reg(handler, IS31FL3731_rows[i], 0x00)) return false;
	}

	return true;
}

bool IS31FL3731_breath_control(I2C_HandleTypeDef* handler, uint8_t enable){
	// Select Function Reg
	if (!IS31FL3731_write_reg(handler, COMMAND_REGISTER, BANK_FUNCTIONREG)) return false;

	if (!IS31FL3731_write_reg(handler, BREATH_CONTROL_2, (enable << 4) + EXTINGUISH_TIME)) return false;

	return true;
}

bool IS31FL3731_set_frame(I2C_HandleTypeDef* handler, uint8_t frame){
	// Select Function Reg
	if (!IS31FL3731_write_reg(handler, COMMAND_REGISTER, BANK_FUNCTIONREG)) return false;

	if (!IS31FL3731_write_reg(handler, FRAME_SELECT, frame)) return false;

	return true;
}

bool IS31FL3731_directLEDdrive(I2C_HandleTypeDef* handler, uint8_t pos, uint8_t aData) {
	// set LED
	if (!IS31FL3731_write_reg(handler, IS31FL3731_rows[pos], aData)) return false;

	return true;
}


char IS31FL3731_ascii_to_digit(char c) {
	switch (c) {
		case ('0'):
		case ('o'):
		case ('O'):
				return 0x1+0x2+0x4+0x8+0x10+0x20;
		case ('1'):
				return 0x2+0x4;
		case ('2'):
				return 0x1+0x2+0x40+0x10+0x8;
		case ('3'):
				return 0x1+0x2+0x40+0x4+0x8;
		case ('4'):
				return 0x20+0x40+0x2+0x4;
		case ('5'):
				return 0x1+0x20+0x40+0x4+0x8;
		case ('6'):
				return 0x1+0x20+0x40+0x4+0x8+0x10;
		case ('7'):
				return 0x1+0x2+0x4;
		case ('8'):
				return 0x1+0x2+0x4+0x8+0x10+0x20+0x40;
		case ('9'):
				return 0x1+0x2+0x4+0x8+0x20+0x40;
		case ('A'):
		case ('a'):
				return 0x1+0x2+0x4+0x10+0x20+0x40;
		case ('B'):
		case ('b'):
				return 0x4+0x8+0x10+0x20+0x40;
		case ('C'):
		case ('c'):
				return 0x1+0x8+0x10+0x20;
		case ('D'):
		case ('d'):
				return 0x2+0x4+0x8+0x10+0x40;
		case ('E'):
		case ('e'):
				return 0x1+0x8+0x10+0x20+0x40;
		case ('F'):
		case ('f'):
				return 0x1+0x10+0x20+0x40;
		case ('H'):
		case ('h'):
				return 0x4+0x10+0x20+0x40;
		case ('-'):
				return 0x40;
		case ('_'):
				return 0x8;
		case (' '):
				return 0x0;
		default: return 0x40; // '-'
	}
}

