/*
 * MAC.h
 *
 *  Created on: May 15, 2024
 *      Author: lepikhov
 */

#ifndef INC_DEVICE_H_
#define INC_DEVICE_H_

enum DEVICE_TYPE {
	DEVICE_TYPE_KDS,
	DEVICE_TYPE_BTU,
	DEVICE_TYPE_UNKWOWN,
};



#define MAC_SIZE 6
#define DEVICE_VERSION 0x10
#define MAXIMUM_PACKET_LENGTH 64

uint8_t copy_MAC(uint8_t* dst);

void check_device_type_func();
enum DEVICE_TYPE get_device_type();

#endif /* INC_DEVICE_H_ */