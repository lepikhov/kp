/*
 * protocol.h
 *
 *  Created on: Apr 23, 2024
 *      Author: lepikhov
 */

#ifndef INC_COMMUNICATION_COMMUNICATION_H_
#define INC_COMMUNICATION_COMMUNICATION_H_

#include <stdint.h>
#include <communication/rxtx_state_mashine.h>
#include <communication/crc.h>

enum COMMUNICATION_STATES {
	COMMUNICATION_OK,
	COMMUNICATION_ERR,
	COMMUNICATION_TX_BUFFER_OVER_ERR,
	COMMUNICATION_RX_BUFFER_OVER_ERR,
	COMMUNICATION_RX_FRAME_ERR,
	COMMUNICATION_RX_PACKET_ERR,
	COMMUNICATION_RX_PACKET_CRC_ERR,
	COMMUNICATION_RX_PACKET_COMMAND_ERR,
};

enum COMMUNICATION_COMMAND_STATES {
	COMMUNICATION_COMMAND_OK,
	COMMUNICATION_COMMAND_MISMATCH,
	COMMUNICATION_COMMAND_ERR,
};

#define COMMUNICATION_BUFFER_SIZE 512

struct communication_struct_type{
	enum RXTX_STATE_MACHINE_STATES state;
	enum RX_STATE_MACHINE_STATES rx_state;
	uint8_t rx_buff[COMMUNICATION_BUFFER_SIZE];
	uint8_t rx_packet_buff[COMMUNICATION_BUFFER_SIZE];
	uint8_t tx_packet_buff[COMMUNICATION_BUFFER_SIZE];
	uint8_t tx_buff[COMMUNICATION_BUFFER_SIZE];
	uint16_t rx_cntr;
	uint16_t tx_size;
	uint16_t rx_size;
	uint16_t rx_packet_size;
	uint16_t tx_packet_size;
};

// protocol frame bytes
#define BYTE_B 0x7E
#define BYTE_H 0x0F
#define BYTE_K 0xF0

enum COMMUNICATION_STATES  communication_init(void);
enum COMMUNICATION_STATES  communication_func(void);
enum COMMUNICATION_STATES  communication_set_tx_buffer(void * src, uint16_t size);
enum COMMUNICATION_STATES  communication_start_tx(void);
enum COMMUNICATION_STATES  communication_start_rx(void);
enum COMMUNICATION_STATES  communication_rx_irq(void);
enum COMMUNICATION_STATES  communication_rx(void);
enum COMMUNICATION_STATES  communication_rx(void);
enum COMMUNICATION_STATES  communication_prepare_rx_packet(void);

void usart_tx_callback(void);
void usart_rx_callback(void);

#endif /* INC_COMMUNICATION_COMMUNICATION_H_ */
