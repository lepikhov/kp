/*
 * device.c
 *
 *  Created on: May 15, 2024
 *      Author: lepikhov
 */


#include <stdint.h>
#include <string.h>
#include "main.h"
#include <device.h>


const char DEVICE_NAME_KDS[] = "KDS";
const char DEVICE_NAME_BTU[] = "BTU";
const char DEVICE_NAME_UNKNOWN[] = "UNKNOWN";
const char COMPILATION_DATE[] = __DATE__" "__TIME__;


uint8_t device_type = 0;
struct work_time_t work_time = {0};

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

uint8_t copy_MAC(uint8_t* dst) {
	memcpy(dst, (uint8_t*)UID_BASE, MAC_SIZE);
	return MAC_SIZE;
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

uint32_t get_work_time_sek() {
	return work_time.sek;
}


