/*
 * MAC.c
 *
 *  Created on: May 15, 2024
 *      Author: lepikhov
 */


#include <stdint.h>
#include <string.h>
#include "main.h"
#include <device.h>

uint8_t device_type = 0;

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
	return DEVICE_TYPE_UNKWOWN;
}

uint8_t copy_MAC(uint8_t* dst) {
	memcpy(dst, (uint8_t*)UID_BASE, MAC_SIZE);
	return MAC_SIZE;
}



