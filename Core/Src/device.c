/*
 * device.c
 *
 *  Created on: May 15, 2024
 *      Author: lepikhov
 */


#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "main.h"
#include <device.h>
#include <communication/communication.h>
#include <communication/crc.h>


const char DEVICE_NAME_KDS[] = "KDS";
const char DEVICE_NAME_BTU[] = "BTU";
const char DEVICE_NAME_UNKNOWN[] = "UNKNOWN";
const char COMPILATION_DATE[] = __DATE__" "__TIME__;


uint8_t device_type = 0;
struct work_time_t work_time = {0};
struct reset_t reset = {0};

void check_device_type_func() {
	if( HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_14) == GPIO_PIN_SET ) {
		device_type |= 0x1;
	}
	else {
		device_type &= ~0x1;
	}
	if( HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13) == GPIO_PIN_SET ) {
		device_type |= 0x2;
	}
	else {
		device_type &= ~0x2;
	}
	if( HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_8) == GPIO_PIN_SET ) {
		device_type |= 0x4;
	}
	else {
		device_type &= ~0x4;
	}

}

enum DEVICE_TYPE get_device_type() {
	if (device_type == 0x1) return DEVICE_TYPE_KDS;
	if (device_type == 0x7) return DEVICE_TYPE_BTU;
	return DEVICE_TYPE_UNKNOWN;
}

uint8_t copy_MAC(char* dst) {
	memcpy(dst, (char*)UID_BASE, MAC_SIZE);
	return MAC_SIZE;
}

bool check_MAC(char* src) {
	return !strncmp((char*)UID_BASE, src, MAC_SIZE);
}

uint8_t copy_device_name(uint8_t* dst) {
	enum DEVICE_TYPE type = get_device_type();
	if (type == DEVICE_TYPE_KDS) {
		strcpy((char *)dst, DEVICE_NAME_KDS);
		return strlen(DEVICE_NAME_KDS);
	}
	else if (type == DEVICE_TYPE_BTU) {
		strcpy((char *)dst, DEVICE_NAME_BTU);
		return strlen(DEVICE_NAME_BTU);
	}
	else {
		strcpy((char *)dst, DEVICE_NAME_UNKNOWN);
		return strlen(DEVICE_NAME_UNKNOWN);
	}
}

uint8_t copy_compilation_date(uint8_t* dst) {
	strcpy((char *)dst, COMPILATION_DATE);
	return strlen(COMPILATION_DATE);
}

void update_work_time() {
	++work_time.mlsek;
	if (work_time.mlsek > 999) {
		++work_time.sek;
		work_time.mlsek = 0;
	}

}

uint32_t get_delta_tick(uint32_t tick) {
	return HAL_GetTick() - tick;
}

uint32_t get_work_time_sek() {
	return work_time.sek;
}


void program_reset_start(uint32_t delay) {
	reset.start = true;
	reset.start_tick = HAL_GetTick();
	reset.delay = delay;
}

void program_reset_func(void) {
	if (reset.start) {
		if (get_delta_tick(reset.start_tick) >= reset.delay) {
			NVIC_SystemReset();
		}
	}
}

uint8_t get_serial_number(uint8_t* dst) {
	FEE_ReadData(SERIAL_NUMBER_START_ADDRESS, dst, SERIAL_NUMBER_WITH_CRC_SIZE);
	return SERIAL_NUMBER_WITH_CRC_SIZE;
}

void set_serial_number(uint8_t* src) {
	FEE_WriteData(SERIAL_NUMBER_START_ADDRESS, src, SERIAL_NUMBER_WITH_CRC_SIZE);
}


uint8_t get_configuration(uint8_t* dst) {
	FEE_ReadData(CONFIGURATION_START_ADDRESS, dst, CONFIGURATION_WITH_CRC_SIZE);
	return CONFIGURATION_WITH_CRC_SIZE;
}

void set_configuration(uint8_t* src) {
	FEE_WriteData(CONFIGURATION_START_ADDRESS, src, CONFIGURATION_WITH_CRC_SIZE);
}

uint8_t get_address(void) {
	struct configuration_t conf;
	get_configuration((uint8_t*)&conf);
	if (crc16_check_buff((uint8_t*)&conf, sizeof(conf), 0xFFFF)) return conf.address;
	return DEFAULT_ADDRESS;
}

