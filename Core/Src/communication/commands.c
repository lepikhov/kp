/*
 * commands.c
 *
 *  Created on: May 8, 2024
 *      Author: Pavel
 */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <communication/communication.h>
#include <communication/commands.h>
#include <device.h>
#include <inputs.h>

communication_commands_func communication_commands_table[] = {
		communication_command_identification,
		communication_command_MAC,
		communication_command_inputs_state,
};

enum COMMUNICATION_COMMAND_STATES communication_command_identification(
		uint8_t* req_packet_buff,
		uint8_t* ans_packet_buff,
		uint16_t req_packet_size,
		uint16_t* ans_packet_size
) {

	//check command

	if (
		(req_packet_buff[2] == COMMAND_IDENTIFICATION) && 	// packet type
		(req_packet_buff[3] == 0x01) && 	// number of block
		(req_packet_buff[4] == 0x01)		// quantity of blocks
	) {

		// this is IDENTIFICATION command

		// prepare answer

		*ans_packet_buff++ = req_packet_buff[1]; // address of the recipient
		*ans_packet_buff++ = req_packet_buff[0]; // address of the sender

		*ans_packet_buff++ = TICKET_IDENTIFICATION;  // packet type
		*ans_packet_buff++ = 0x01; // number of block
		*ans_packet_buff++ = 0x01; // quantity of blocks

		*ans_packet_buff++ = DEVICE_MANUFACTURE_ID; // device manufacturer

		// device type
		enum DEVICE_TYPE type = get_device_type();
		if (type == DEVICE_TYPE_KDS) *ans_packet_buff++ = DEVICE_TYPE_KDS_ID;
		else if (type == DEVICE_TYPE_BTU) *ans_packet_buff++ = DEVICE_TYPE_BTU_ID;
		else *ans_packet_buff++ = DEVICE_TYPE_UNKNOWN_ID;


		*ans_packet_buff++ = DEVICE_VERSION; // device version
		*ans_packet_buff++ = MAXIMUM_PACKET_LENGTH; // maximum packet length
		*ans_packet_buff++ = 2; 	// maximum waiting time for a response from the device in tens
									// of milliseconds (*10 msec)


		*ans_packet_size = 10;

		return COMMUNICATION_COMMAND_OK;

	}

	// this is not IDENTIFICATION command

	return COMMUNICATION_COMMAND_MISMATCH;
}

enum COMMUNICATION_COMMAND_STATES communication_command_MAC(
		uint8_t* req_packet_buff,
		uint8_t* ans_packet_buff,
		uint16_t req_packet_size,
		uint16_t* ans_packet_size
) {

	//check command

	if (
		(req_packet_buff[2] == COMMAND_DATA_REQUEST) && 	// packet type
		(req_packet_buff[3] == 0x01) && 	// number of block
		(req_packet_buff[4] == 0x01) &&		// quantity of blocks
		(req_packet_buff[5] == COMMAND_ID_MAC)
	) {

		// this is MAC command

		// prepare answer

		*ans_packet_buff++ = req_packet_buff[1]; // address of the recipient
		*ans_packet_buff++ = req_packet_buff[0]; // address of the sender

		*ans_packet_buff++ = TICKET_DATA_SEND; // packet type
		*ans_packet_buff++ = 0x01; // number of block
		*ans_packet_buff++ = 0x01; // quantity of blocks

		*ans_packet_buff++ = TICKET_ID_MAC; //

		*ans_packet_size = 6 + copy_MAC(ans_packet_buff);

		return COMMUNICATION_COMMAND_OK;

	}

	// this is not MAC command

	return COMMUNICATION_COMMAND_MISMATCH;
}

enum COMMUNICATION_COMMAND_STATES communication_command_inputs_state(
		uint8_t* req_packet_buff,
		uint8_t* ans_packet_buff,
		uint16_t req_packet_size,
		uint16_t* ans_packet_size
) {

	uint16_t inputs;
	//check command

	if (
		(req_packet_buff[2] == COMMAND_DATA_REQUEST) && 	// packet type
		(req_packet_buff[3] == 0x01) && 	// number of block
		(req_packet_buff[4] == 0x01) &&		// quantity of blocks
		(req_packet_buff[5] == COMMAND_ID_INPUTS_STATE)
	) {

		// this is inputs state command

		// prepare answer

		*ans_packet_buff++ = req_packet_buff[1]; // address of the recipient
		*ans_packet_buff++ = req_packet_buff[0]; // address of the sender

		*ans_packet_buff++ = TICKET_DATA_SEND; // packet type
		*ans_packet_buff++ = 0x01; // number of block
		*ans_packet_buff++ = 0x01; // quantity of blocks

		*ans_packet_buff++ = TICKET_ID_DATA;

		inputs = inputs_get_data(true);
		*ans_packet_buff++ = inputs&0xff; // 0..7 in
		*ans_packet_buff++ = (inputs>>8)&0xff; // 8..15 in


		*ans_packet_size = 8;

		return COMMUNICATION_COMMAND_OK;

	}

	// this is not MAC command

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

	if (func_pars_state != COMMUNICATION_COMMAND_OK) {
		switch (func_pars_state) {
			case COMMUNICATION_COMMAND_MISMATCH:
				//
				communication_command_error_response(
						TICKET_WRONG_COMMAND, 	//wrong command ticket
						req_packet_buff,
						ans_packet_buff,
						ans_packet_size
						);
				break;
			case COMMUNICATION_COMMAND_PACKET_ERR:
				//
				communication_command_error_response(
						TICKET_WRONG_PACKET_DATA, 	//wrong data in packet ticket
						req_packet_buff,
						ans_packet_buff,
						ans_packet_size
						);
				break;
			default:
				//s
		}
		state = COMMUNICATION_RX_PACKET_COMMAND_ERR;
	}

	return state;
}

enum COMMUNICATION_COMMAND_STATES communication_command_error_response(
		uint8_t ticket,
		uint8_t* req_packet_buff,
		uint8_t* ans_packet_buff,
		uint16_t* ans_packet_size
		) {

	*ans_packet_buff++ = req_packet_buff[1]; // address of the recipient
	*ans_packet_buff++ = req_packet_buff[0]; // address of the sender

	*ans_packet_buff++ = ticket; // packet type


	*ans_packet_size = 3;

	return COMMUNICATION_OK;
}
