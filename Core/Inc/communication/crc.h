/*
 * crc.h
 *
 *  Created on: May 7, 2024
 *      Author: Pavel
 */

#ifndef CRC_H_
#define CRC_H_

#include <stdbool.h>

/*
  CRC-16/XMODEM
  Poly  : 0x1021    x^16 + x^12 + x^5 + 1
  Init  : 0x0000
  Revert: false
  XorOut: 0x0000
  Check : 0x31C3 ("123456789")
  MaxLen: 4095 байт (32767 бит)
*/

#define CRC_POLY 0x1021

uint16_t crc16_calc(uint8_t data, uint16_t crc);
void crc16_calc_buff(uint8_t *buff, uint16_t count, uint16_t init);
bool crc16_check_buff(uint8_t *buff, uint16_t count, uint16_t init);


#endif /* CRC_H_ */
