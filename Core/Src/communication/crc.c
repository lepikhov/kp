/*
 * crc.c
 *
 *  Created on: May 7, 2024
 *      Author: Pavel
 */

#include <stdint.h>
#include <communication/crc.h>


uint16_t crc16_calc(uint8_t data, uint16_t crc) {
	crc ^= ((uint16_t)data << 8);
	for (uint8_t i=0; i<8; i++)
	{
		crc = crc & 0x8000 ? (crc << 1) ^ CRC_POLY : crc << 1;
	}

	return crc;
}

void crc16_calc_buff(uint8_t *buff, uint16_t count) {
	uint16_t crc = 0x0000;
	while(count--) {
		crc = crc16_calc(*buff++, crc);
	}
	*buff++ = crc >> 8;		// high byte
	*buff++ = crc & 0xff;	// low byte
}

bool crc16_check_buff(uint8_t *buff, uint16_t count) {
	uint16_t crc = 0x0000;
	while(count--) {
		crc = crc16_calc(*buff++, crc);
	}
	return (bool)!crc;
}




