/*
 * commands.c
 *
 *  Created on: May 8, 2024
 *      Author: Pavel
 */
#include <stdint.h>
#include <communication/communication.h>
#include <communication/commands.h>

communication_commands_func communication_commands_table[] = {
		communication_command_identification,
};

enum COMMUNICATION_COMMAND_STATES communication_command_identification(
		uint8_t* req_packet_buff,
		uint8_t* ans_packet_buff,
		uint16_t req_packet_size,
		uint16_t* ans_packet_size
) {
	if (
		(req_packet_buff[2] == 0x80) &&
		(req_packet_buff[3] == 0x01) &&
		(req_packet_buff[4] == 0x01)
	) {
		*ans_packet_buff++ = req_packet_buff[1];
		*ans_packet_buff++ = req_packet_buff[0];

		*ans_packet_buff++ = 0x0;
		*ans_packet_buff++ = 0x37;
		*ans_packet_buff++ = 0x10;
		*ans_packet_buff++ = 0x10;
		*ans_packet_buff++ = 255;
		*ans_packet_buff++ = 10;

		*ans_packet_size = 8;

		return COMMUNICATION_COMMAND_OK;

	}

	return COMMUNICATION_COMMAND_MISMATCH;
}

enum COMMUNICATION_STATES communication_commands_parser(
		uint8_t* req_packet_buff,
		uint8_t* ans_packet_buff,
		uint16_t req_packet_size,
		uint16_t* ans_packet_size
		) {

	enum COMMUNICATION_STATES state = COMMUNICATION_OK;
	enum COMMUNICATION_COMMAND_STATES func_pars_state = COMMUNICATION_OK;

	for (uint16_t i=0; i<sizeof(communication_commands_table)/sizeof(communication_commands_func); ++i) {
		func_pars_state = communication_commands_table[i](req_packet_buff, ans_packet_buff, req_packet_size, ans_packet_size);
		if (func_pars_state != COMMUNICATION_COMMAND_MISMATCH) break;
	}

	if (func_pars_state != COMMUNICATION_COMMAND_OK) state = COMMUNICATION_RX_PACKET_COMMAND_ERR;

	return state;
}
