/*
 * indication.c
 *
 *  Created on: May 27, 2024
 *      Author: lepikhov
 */

#include <stdint.h>
#include <stdbool.h>
#include "main.h"
#include <indication/i2c_er.h>
#include <indication/tm1650.h>
#include <indication/IS31FL3731.h>
#include "i2c.h"
#include <indication/indication.h>
#include <device.h>


struct indication_t indication={0};
struct indication_rxtx_t indication_rxtx={0};



void indication_init(void) {
	HAL_I2C_MspInit(&hi2c2);
	I2C_ClearBusyFlagErratum(&hi2c2, 10);
}

void indication_func(void) {

	if (indication.type == INDICATION_TYPE_TM1650) indication_func_TM1650();
	else indication_func_IS31FL3731();
}

void indication_func_IS31FL3731(void) {

	uint8_t d;

	switch (indication.state) {
		case INDIICATION_STATE_IDLE:
			//
			if (!IS31FL3731_init(&hi2c2)) {
				I2C_ClearBusyFlagErratum(&hi2c2, 10);
				if (++indication.connect_try_count>MAX_CONNECT_TRY_COUNT) {
					indication.type = INDICATION_TYPE_TM1650;
					indication.connect_try_count = 0;
				}
				return;
			}
			IS31FL3731_write_reg(&hi2c2, COMMAND_REGISTER, BANK_1);
			indication.start_tick=HAL_GetTick();
			indication.state=INDICATION_STATE_START;
			break;
		case INDICATION_STATE_START:
			if (get_delta_tick(indication.start_tick)>=INDICATION_START_DELAY) {
				indication.state=INDICATION_STATE_LEDS1;
			}
			break;
		case INDICATION_STATE_LEDS1:
			//
			IS31FL3731_directLEDdrive(&hi2c2, 0, indication.leds[0]);
			indication.state=INDICATION_STATE_LEDS2;
			break;
		case INDICATION_STATE_LEDS2:
			//
			IS31FL3731_directLEDdrive(&hi2c2, 1, indication.leds[1]);
			indication.state=INDICATION_STATE_LEDS3;
			break;
		case INDICATION_STATE_LEDS3:
			//
			IS31FL3731_directLEDdrive(&hi2c2, 2, indication.leds[2]);
			indication.state=INDICATION_STATE_LEDS4;
			break;
		case INDICATION_STATE_LEDS4:
			//
			IS31FL3731_directLEDdrive(&hi2c2, 3, indication.leds[3]);
			indication.state=INDICATION_STATE_DIGIT1;
			break;
		case INDICATION_STATE_DIGIT1:
			//
			d = get_address()/0x10;
			d = d<0xA ? d+0x30 : d+0x37;
			IS31FL3731_directLEDdrive(&hi2c2, DIGIT1_POSITION, IS31FL3731_ascii_to_digit(d));
			indication.state=INDICATION_STATE_DIGIT2;
			break;
		case INDICATION_STATE_DIGIT2:
			//
			d = get_address()%0x10;
			d = d<0xA ? d+0x30 : d+0x37;
			IS31FL3731_directLEDdrive(&hi2c2, DIGIT2_POSITION, IS31FL3731_ascii_to_digit(d));
			indication.state=INDICATION_STATE_DIGIT3;
			break;
		case INDICATION_STATE_DIGIT3:
			//
			IS31FL3731_directLEDdrive(&hi2c2, DIGIT3_POSITION, IS31FL3731_ascii_to_digit('h'));
			indication.state=INDICATION_STATE_LEDS1;
			break;
		default:
			//
			indication.state=INDICATION_STATE_LEDS1;
	}
}

void indication_func_TM1650(void) {

	switch (indication.state) {
		case INDIICATION_STATE_IDLE:
			//
			TM1650_TM1650(4);
			if (!TM1650_init()) {
				I2C_ClearBusyFlagErratum(&hi2c2, 10);
				if (++indication.connect_try_count>MAX_CONNECT_TRY_COUNT) {
					indication.type = INDICATION_TYPE_IS31FL3731;
					indication.connect_try_count = 0;
				}
				return;
			}
			TM1650_displayOn();
			TM1650_setBrightness(TM1650_MAX_BRIGHT);
			indication.state=INDICATION_STATE_LEDS1;
			break;
		case INDICATION_STATE_LEDS1:
			//
			TM1650_directLEDdrive(0, indication.leds[0]);
			indication.state=INDICATION_STATE_LEDS2;
			break;
		case INDICATION_STATE_LEDS2:
			//
			TM1650_directLEDdrive(1, indication.leds[1]);
			indication.state=INDICATION_STATE_LEDS3;
			break;
		case INDICATION_STATE_LEDS3:
			//
			TM1650_directLEDdrive(2, indication.leds[2]);
			indication.state=INDICATION_STATE_LEDS4;
			break;
		case INDICATION_STATE_LEDS4:
			//
			TM1650_directLEDdrive(3, indication.leds[3]);
			indication.state=INDICATION_STATE_LEDS1;
			break;
		default:
			//
			indication.state=INDICATION_STATE_LEDS1;
	}


}

void indication_set_leds(uint8_t addr, uint8_t mean) {
	if (addr > DIGITS_NUMBER-1) return;
	indication.leds[addr]=mean;
}

uint8_t indication_get_leds(uint8_t addr) {
	if (addr > DIGITS_NUMBER-1) return 0;
	return indication.leds[addr];
}

void indication_set_led(uint8_t addr) {
	if (addr > LEDS_NUMBER-1) return;
	uint8_t dig = addr/8;
	uint8_t led = 1 << (addr%8);
	uint8_t tmp = indication_get_leds(dig) | led;
	indication_set_leds(dig, tmp);
}

void indication_clear_led(uint8_t addr) {
	if (addr > LEDS_NUMBER-1) return;
	uint8_t dig = addr/8;
	uint8_t led = 1 << (addr%8);
	uint8_t tmp = indication_get_leds(dig) & (~led);
	indication_set_leds(dig, tmp);
}

void indication_start_rx_led(void) {
	indication_rxtx.rx_counter = RX_TX_LED_COUNTER_LOADER;
	indication_rxtx.rx_flag = true;
	indication_set_led(COMMUNICATION_RX_LED);
}

void indication_start_tx_led(void) {
	indication_rxtx.tx_counter = RX_TX_LED_COUNTER_LOADER;
	indication_rxtx.tx_flag = true;
	indication_set_led(COMMUNICATION_TX_LED);
}

void indication_rx_tx_led_func(void) {
	if (indication_rxtx.rx_flag && indication_rxtx.rx_counter--) {
		if (!indication_rxtx.rx_counter) indication_clear_led(COMMUNICATION_RX_LED);
	}
	if (indication_rxtx.tx_flag && indication_rxtx.tx_counter--) {
		if (!indication_rxtx.tx_counter) indication_clear_led(COMMUNICATION_TX_LED);
	}
}

void indication_health_led_func(bool set) {
	if (set) {
		indication_set_led(DEVICE_HEALTH_LED);
	}
	else {
		indication_clear_led(DEVICE_HEALTH_LED);
	}
}

