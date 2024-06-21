/*
 * MAC.h
 *
 *  Created on: May 15, 2024
 *      Author: lepikhov
 */

#ifndef INC_DEVICE_H_
#define INC_DEVICE_H_

#include "../../MIDWARE/FEE/FEE.h"

enum DEVICE_TYPE {
	DEVICE_TYPE_KDS,
	DEVICE_TYPE_BTU,
	DEVICE_TYPE_UNKNOWN,
};

struct work_time_t {
	uint16_t mlsek;
	uint32_t sek;
};

struct reset_t {
	bool start;
	uint32_t start_tick;
	uint32_t delay;
};


#define MAC_SIZE 6
#define DEVICE_VERSION 0x10
#define MAXIMUM_PACKET_LENGTH 64

#define PROGRAM_RESET_DELAY 1000 // 1 sek


uint8_t copy_MAC(char* dst);
bool check_MAC(char* src);


uint8_t copy_device_name(uint8_t* dst);
uint8_t copy_compilation_date(uint8_t* dst);

void check_device_type_func();
enum DEVICE_TYPE get_device_type();

void update_work_time();
uint32_t get_work_time_sek();
uint32_t get_delta_tick(uint32_t tick);

void program_reset_start(uint32_t delay);
void program_reset_func(void);

// read/write some system data from/to flash

#define SERIAL_NUMBER_START_ADDRESS FEE_START_ADDRESS //serial number address

// read/write serial number
#define SERIAL_NUMBER_WITH_CRC_SIZE (sizeof(uint32_t)+sizeof(uint16_t))
uint8_t get_serial_number(uint8_t* dst);
void set_serial_number(uint8_t* src);

// read/write configuration
#define CONFIGURATION_START_ADDRESS (FEE_START_ADDRESS+SERIAL_NUMBER_WITH_CRC_SIZE)

typedef struct configuration_t {
	uint8_t address;
	uint8_t buffer[13];
	uint16_t crc;
} configuration;

#define CONFIGURATION_WITH_CRC_SIZE sizeof(configuration)
uint8_t get_configuration(uint8_t* dst);
void set_configuration(uint8_t* src);

//RS485 address
uint8_t get_address();


#endif /* INC_DEVICE_H_ */
