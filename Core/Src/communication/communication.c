/*
 * protocol.c
 *
 *  Created on: Apr 23, 2024
 *      Author: lepikhov
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <communication/communication.h>
#include <communication/commands.h>
#include <indication/indication.h>
#include <device.h>
#include "main.h"
#include "usart.h"
#include "board.h"


struct communication_t communication={0};
struct communication_statistic_t communication_statistic={0};



enum COMMUNICATION_STATES communication_init(void) {
	communication_set_address(get_address());
	return communication_start_rx();
}

enum COMMUNICATION_STATES communication_start_rx(void) {
	RS485_DIR_RX;
	communication.rx_cntr = 0;
	communication.rx_size = 0;
	communication.state = RXTX_STATE_MACHINE_RECEIVING;
	communication.rx_state = RX_STATE_MACHINE_WAITING_START_B;
	HAL_UART_Receive_IT (&huart2, (uint8_t*)communication.rx_buff, 1);
	return COMMUNICATION_OK;
}

enum COMMUNICATION_STATES communication_func(void) {

	switch (communication.state) {

		case RXTX_STATE_MACHINE_IDLE:
			//
			break;
		case RXTX_STATE_MACHINE_RECEIVING:
			//
			HAL_NVIC_DisableIRQ(USART2_IRQn);
			communication_rx();
			HAL_NVIC_EnableIRQ(USART2_IRQn);
			break;
		case RXTX_STATE_MACHINE_RECEIVED:
			//
			indication_start_rx_led();
			++communication_statistic.tx_count;

			///* if need temporary disable check address



			if (!communication_check_address(communication.rx_packet_buff[0],
											 communication.rx_packet_buff[1]
											)
				) {
				// message for not this device address
				communication_start_rx();
				break;
			}

			//*/

			if (communication_commands_parser(
					communication.rx_packet_buff,
					communication.tx_packet_buff,
					communication.rx_packet_size,
					&communication.tx_packet_size
					) != COMMUNICATION_OK) ++communication_statistic.tx_err_count;

			crc16_calc_buff(
					communication.tx_packet_buff,
					communication.tx_packet_size,
					0x0000
			);

			communication.tx_packet_size += 2;

			communication_prepare_tx_buffer();

			communication_set_tx_buffer(
					communication.tx_buff,
					communication.tx_size
			);

			communication_start_tx();
			break;
		case RXTX_STATE_MACHINE_SENDING_GOOD_RESPONSE:
			//
			break;
		case RXTX_STATE_MACHINE_SENDING_ERROR_RESPONSE:
			//
			break;
		default:
			//
	}


	return COMMUNICATION_OK;
}

enum COMMUNICATION_STATES  communication_rx(void){
	enum COMMUNICATION_STATES state = COMMUNICATION_OK;

	switch (communication.rx_state) {
		case RX_STATE_MACHINE_RACEIVED_FULL_POCKET:
			//
			++communication_statistic.rx_count;
			state = communication_prepare_rx_packet();
			if (state != COMMUNICATION_OK) {
				++communication_statistic.rx_err_count;
				communication.state = RXTX_STATE_MACHINE_SENDING_ERROR_RESPONSE;
				return COMMUNICATION_RX_PACKET_ERR;
			}
			if (!crc16_check_buff(communication.rx_packet_buff,
					communication.rx_packet_size,
					0x0000)
				) {
				++communication_statistic.rx_err_count;
				communication_start_rx();
				return COMMUNICATION_RX_PACKET_CRC_ERR;
			}
			communication.state = RXTX_STATE_MACHINE_RECEIVED;
			break;
		case RX_STATE_MACHINE_RACEIVED_CRASHED_POCKET:
			//
			break;
		default:
			//
	}
	return state;
}

enum COMMUNICATION_STATES communication_rx_irq(void) {

	enum COMMUNICATION_STATES state = COMMUNICATION_OK;
	uint16_t rx_cntr = communication.rx_cntr;

	switch (communication.rx_state) {
		case RX_STATE_MACHINE_WAITING_START_B:
			//
			if (communication.rx_buff[rx_cntr] == BYTE_B) {
				communication.rx_state = RX_STATE_MACHINE_WAITING_START_H;
			}
			else {
				communication.rx_cntr = 0;
				++communication_statistic.rx_err_count;
				state = COMMUNICATION_RX_FRAME_ERR;
			}
			break;
		case RX_STATE_MACHINE_WAITING_START_H:
			//
			if (communication.rx_buff[rx_cntr] == BYTE_H) {
				communication.rx_state = RX_STATE_MACHINE_RACEIVING_POCKET;
			}
			else {//Not 'H'
				communication.rx_cntr = 0;
				communication.rx_state = RX_STATE_MACHINE_WAITING_START_B;
			}
			break;
		case RX_STATE_MACHINE_RACEIVING_POCKET:
			//
			if (communication.rx_buff[rx_cntr] == BYTE_B) {
				communication.rx_state = RX_STATE_MACHINE_RACEIVED_B_IN_POCKET;
			}
			break;
		case RX_STATE_MACHINE_RACEIVED_B_IN_POCKET:
			//
			if (communication.rx_buff[rx_cntr] == BYTE_B) {
				communication.rx_state = RX_STATE_MACHINE_RACEIVING_POCKET;
			}
			else {
				if (communication.rx_buff[rx_cntr] == BYTE_K) {
					communication.rx_state = RX_STATE_MACHINE_RACEIVED_FULL_POCKET;
					communication.rx_size = communication.rx_cntr + 1;
				}
				else {//Not 'B' and Not 'K'
					communication.rx_cntr = 0;
					++communication_statistic.rx_err_count;
					communication.rx_state = RX_STATE_MACHINE_WAITING_START_B;
				}
			}
			break;
		case RX_STATE_MACHINE_RACEIVED_FULL_POCKET:
			//
			break;
		case RX_STATE_MACHINE_RACEIVED_CRASHED_POCKET:
			//
			break;
		default:
			//
	}

	if ( state == COMMUNICATION_OK) {
		if (rx_cntr < COMMUNICATION_BUFFER_SIZE-1) ++communication.rx_cntr;
		else {
			communication.rx_cntr = 0;
			++communication_statistic.rx_err_count;
			state = COMMUNICATION_RX_BUFFER_OVER_ERR;
		}
	}

	return state;
}

enum COMMUNICATION_STATES communication_set_tx_buffer(void * src, uint16_t size) {
	int8_t status = COMMUNICATION_OK;
	if (size > COMMUNICATION_BUFFER_SIZE) return COMMUNICATION_TX_BUFFER_OVER_ERR;
	memcpy(communication.tx_buff, src, size);
	communication.tx_size = size;
	return status;
}

enum COMMUNICATION_STATES communication_start_tx(void) {
	RS485_DIR_TX;
	indication_start_tx_led();
	communication.state = RXTX_STATE_MACHINE_SENDING_GOOD_RESPONSE;
	HAL_UART_Transmit_IT(&huart2, (uint8_t*)communication.tx_buff, communication.tx_size);
	return COMMUNICATION_OK;
}

void usart_tx_callback(void) {
	if (communication.state == RXTX_STATE_MACHINE_SENDING_GOOD_RESPONSE) {
		//++communication_statistic.tx_count;
		communication_start_rx();
	}
}

void usart_rx_callback(void) {
	if (communication.state == RXTX_STATE_MACHINE_RECEIVING) {
		communication_rx_irq();
		HAL_UART_Receive_IT (&huart2, (uint8_t*)&communication.rx_buff[communication.rx_cntr], 1);
	}
}

enum COMMUNICATION_STATES  communication_prepare_rx_packet(void) {
	uint8_t * src = &communication.rx_buff[2];
	uint8_t * dst = communication.rx_packet_buff;
	uint16_t cntr = communication.rx_size;

	communication.rx_packet_size = 0;

	if (cntr < 4) return COMMUNICATION_RX_PACKET_ERR;

	cntr -= 4;

	while (cntr--) {
		if (*src == BYTE_B) {
			++src;
			--cntr;
		}
		*dst++ =  *src++;
		++communication.rx_packet_size;
	}

	return COMMUNICATION_OK;

}

enum COMMUNICATION_STATES  communication_prepare_tx_buffer(void) {
	uint8_t * src = communication.tx_packet_buff;
	uint8_t * dst = communication.tx_buff;
	uint16_t cntr = communication.tx_packet_size;

	communication.tx_size = 4;

	*dst++ = BYTE_B;
	*dst++ = BYTE_H;

	while (cntr--) {
		if (*src == BYTE_B) {
			++communication.tx_size;
			*dst++ = BYTE_B;
		}
		*dst++ =  *src++;
		++communication.tx_size;
	}

	*dst++ = BYTE_B;
	*dst++ = BYTE_K;


	return COMMUNICATION_OK;

}

uint8_t communication_copy_statistic(uint8_t * dst) {
	memcpy(dst, &communication_statistic, sizeof(communication_statistic));
	return sizeof(communication_statistic);
}


void communication_set_address(uint8_t address) {
	communication.address = address;
}

bool communication_check_address(uint8_t recipient_address, uint8_t sender_address) {
	enum DEVICE_TYPE type = get_device_type();
	if (type==DEVICE_TYPE_BTU) recipient_address &= ~0x60;
	return (sender_address == 0xfe) ||
		   (recipient_address == 0xff) ||
		   (communication.address == recipient_address);
}
