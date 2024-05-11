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
#include "main.h"
#include "usart.h"
#include "board.h"


struct communication_struct_type communication_struct={0};

enum COMMUNICATION_STATES communication_init(void) {
	return communication_start_rx();
}

enum COMMUNICATION_STATES communication_start_rx(void) {
	RS485_DIR_RX;
	communication_struct.rx_cntr = 0;
	communication_struct.rx_size = 0;
	communication_struct.state = RXTX_STATE_MACHINE_RECEIVING;
	communication_struct.rx_state = RX_STATE_MACHINE_WAITING_START_B;
	HAL_UART_Receive_IT (&huart2, (uint8_t*)communication_struct.rx_buff, 1);
	return COMMUNICATION_OK;
}

enum COMMUNICATION_STATES communication_func(void) {

	switch (communication_struct.state) {

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
			/*
			communication_set_tx_buffer(
					communication_struct.rx_packet_buff,
					communication_struct.rx_packet_size
			);
			*/
			communication_commands_parser(
					communication_struct.rx_packet_buff,
					communication_struct.tx_packet_buff,
					communication_struct.rx_packet_size,
					&communication_struct.tx_packet_size
			);

			crc16_calc_buff(
					communication_struct.tx_packet_buff,
					communication_struct.tx_packet_size
			);

			communication_struct.tx_packet_size += 2;

			communication_set_tx_buffer(
					communication_struct.tx_packet_buff,
					communication_struct.tx_packet_size
			);

			communication_start_tx();
			break;
		case RXTX_STATE_MACHINE_SENDING_GOOD_RESPONSE:
			//
			break;
		case RXTX_STATE_MACHINE_SENDING_ERROR_RESPONSE:
			//
			communication_struct.rx_packet_buff[communication_struct.rx_packet_size] = 0xEE;
			communication_set_tx_buffer(
					communication_struct.rx_packet_buff,
					communication_struct.rx_packet_size+1
			);
			communication_start_tx();
			break;
		default:
			//
	}


	return COMMUNICATION_OK;
}

enum COMMUNICATION_STATES  communication_rx(void){
	enum COMMUNICATION_STATES state = COMMUNICATION_OK;

	switch (communication_struct.rx_state) {
		case RX_STATE_MACHINE_RACEIVED_FULL_POCKET:
			//
			state = communication_prepare_rx_packet();
			if (state != COMMUNICATION_OK) {
				communication_struct.state = RXTX_STATE_MACHINE_SENDING_ERROR_RESPONSE;
				return COMMUNICATION_RX_PACKET_ERR;
			}
			if (!crc16_check_buff(communication_struct.rx_packet_buff, communication_struct.rx_packet_size)) {
				communication_struct.state = RXTX_STATE_MACHINE_SENDING_ERROR_RESPONSE;
				return COMMUNICATION_RX_PACKET_CRC_ERR;
			}
			communication_struct.state = RXTX_STATE_MACHINE_RECEIVED;
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
	uint16_t rx_cntr = communication_struct.rx_cntr;

	switch (communication_struct.rx_state) {
		case RX_STATE_MACHINE_WAITING_START_B:
			//
			if (communication_struct.rx_buff[rx_cntr] == BYTE_B) {
				communication_struct.rx_state = RX_STATE_MACHINE_WAITING_START_H;
			}
			else {
				communication_struct.rx_cntr = 0;
				state = COMMUNICATION_RX_FRAME_ERR;
			}
			break;
		case RX_STATE_MACHINE_WAITING_START_H:
			//
			if (communication_struct.rx_buff[rx_cntr] == BYTE_H) {
				communication_struct.rx_state = RX_STATE_MACHINE_RACEIVING_POCKET;
			}
			else {//Not 'H'
				communication_struct.rx_cntr = 0;
				communication_struct.rx_state = RX_STATE_MACHINE_WAITING_START_B;
			}
			break;
		case RX_STATE_MACHINE_RACEIVING_POCKET:
			//
			if (communication_struct.rx_buff[rx_cntr] == BYTE_B) {
				communication_struct.rx_state = RX_STATE_MACHINE_RACEIVED_B_IN_POCKET;
			}
			break;
		case RX_STATE_MACHINE_RACEIVED_B_IN_POCKET:
			//
			if (communication_struct.rx_buff[rx_cntr] == BYTE_B) {
				communication_struct.rx_state = RX_STATE_MACHINE_RACEIVING_POCKET;
			}
			else {
				if (communication_struct.rx_buff[rx_cntr] == BYTE_K) {
					communication_struct.rx_state = RX_STATE_MACHINE_RACEIVED_FULL_POCKET;
					communication_struct.rx_size = communication_struct.rx_cntr + 1;
				}
				else {//Not 'B' and Not 'K'
					communication_struct.rx_cntr = 0;
					communication_struct.rx_state = RX_STATE_MACHINE_WAITING_START_B;
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
		if (rx_cntr < COMMUNICATION_BUFFER_SIZE-1) ++communication_struct.rx_cntr;
		else state = COMMUNICATION_RX_BUFFER_OVER_ERR;
	}

	return state;
}

enum COMMUNICATION_STATES communication_set_tx_buffer(void * src, uint16_t size) {
	int8_t status = COMMUNICATION_OK;
	if (size > COMMUNICATION_BUFFER_SIZE) return COMMUNICATION_TX_BUFFER_OVER_ERR;
	memcpy(communication_struct.tx_buff, src, size);
	communication_struct.tx_size = size;
	return status;
}

enum COMMUNICATION_STATES communication_start_tx(void) {
	RS485_DIR_TX;
	communication_struct.state = RXTX_STATE_MACHINE_SENDING_GOOD_RESPONSE;
	HAL_UART_Transmit_IT(&huart2, (uint8_t*)communication_struct.tx_buff, communication_struct.tx_size);
	return COMMUNICATION_OK;
}

void usart_tx_callback(void) {
	if (communication_struct.state == RXTX_STATE_MACHINE_SENDING_GOOD_RESPONSE) {
		communication_start_rx();
	}
}

void usart_rx_callback(void) {
	if (communication_struct.state == RXTX_STATE_MACHINE_RECEIVING) {
		communication_rx_irq();
		HAL_UART_Receive_IT (&huart2, (uint8_t*)&communication_struct.rx_buff[communication_struct.rx_cntr], 1);
	}
}

enum COMMUNICATION_STATES  communication_prepare_rx_packet(void) {
	uint8_t * src = &communication_struct.rx_buff[2];
	uint8_t * dst = communication_struct.rx_packet_buff;
	uint16_t cntr = communication_struct.rx_size;

	communication_struct.rx_packet_size = 0;

	if (cntr < 4) return COMMUNICATION_RX_PACKET_ERR;

	cntr -= 4;

	while (cntr--) {
		if (*src == BYTE_B) {
			++src;
			--cntr;
		}
		*dst++ =  *src++;
		++communication_struct.rx_packet_size;
	}

	return COMMUNICATION_OK;

}

