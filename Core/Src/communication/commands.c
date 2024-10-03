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
#include <outputs.h>

communication_commands_func communication_commands_table_kds[] = {
		communication_command_identification,
		communication_command_MAC,
		communication_command_reset,
		communication_command_inputs_state,
		communication_command_inputs_state_changed,
		communication_command_inputs_state_blinking,
		communication_command_work_time,
		communication_command_device_name,
		communication_command_compilation_date,
		communication_command_checksum,
		communication_command_statistic,
		communication_command_read_parameters,
		communication_command_write_parameters,
		communication_command_read_configuration,
		communication_command_write_configuration,
};

communication_commands_func communication_commands_table_btu[] = {
		communication_command_identification,
		communication_command_MAC,
		communication_command_reset,
		communication_command_outputs_preliminary,
		communication_command_outputs_executive,
		communication_command_work_time,
		communication_command_device_name,
		communication_command_compilation_date,
		communication_command_checksum,
		communication_command_statistic,
		communication_command_read_parameters,
		communication_command_write_parameters,
		communication_command_read_configuration,
		communication_command_write_configuration,
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

		if (req_packet_size != 7) return COMMUNICATION_COMMAND_PACKET_ERR; //wrong data in packets

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
		*ans_packet_buff = 2; 	// maximum waiting time for a response from the device in tens
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

		if (req_packet_size != 8) return COMMUNICATION_COMMAND_PACKET_ERR; //wrong data in packets

		// prepare answer

		*ans_packet_buff++ = req_packet_buff[1]; // address of the recipient
		*ans_packet_buff++ = req_packet_buff[0]; // address of the sender

		*ans_packet_buff++ = TICKET_DATA_SEND; // packet type
		*ans_packet_buff++ = 0x01; // number of block
		*ans_packet_buff++ = 0x01; // quantity of blocks

		*ans_packet_buff++ = TICKET_ID_MAC; //

		*ans_packet_size = 6 + copy_MAC((char *)ans_packet_buff);

		return COMMUNICATION_COMMAND_OK;

	}

	// this is not MAC command

	return COMMUNICATION_COMMAND_MISMATCH;
}

enum COMMUNICATION_COMMAND_STATES communication_command_reset(
		uint8_t* req_packet_buff,
		uint8_t* ans_packet_buff,
		uint16_t req_packet_size,
		uint16_t* ans_packet_size
) {

	//check command

	if (
		(req_packet_buff[2] == COMMAND_RESET) && 	// packet type
		(req_packet_buff[3] == 0x01) && 	// number of block
		(req_packet_buff[4] == 0x01) 		// quantity of blocks
	) {

		// this is reset command

		if (req_packet_size != 7) return COMMUNICATION_COMMAND_PACKET_ERR; //wrong data in packets

		program_reset_start(PROGRAM_RESET_DELAY);

		// prepare answer

		*ans_packet_buff++ = req_packet_buff[1]; // address of the recipient
		*ans_packet_buff++ = req_packet_buff[0]; // address of the sender

		*ans_packet_buff++ = TICKET_RESET; // packet type
		*ans_packet_buff++ = 0x01; // number of block
		*ans_packet_buff = 0x01; // quantity of blocks


		*ans_packet_size = 5;

		return COMMUNICATION_COMMAND_OK;

	}

	// this is not reset command

	return COMMUNICATION_COMMAND_MISMATCH;
}

enum COMMUNICATION_COMMAND_STATES communication_command_inputs_state(
		uint8_t* req_packet_buff,
		uint8_t* ans_packet_buff,
		uint16_t req_packet_size,
		uint16_t* ans_packet_size
) {

	uint16_t inputs;
	uint8_t * ptr = (uint8_t*)&inputs;

	//check command

	if (
		(req_packet_buff[2] == COMMAND_DATA_REQUEST) && 	// packet type
		(req_packet_buff[3] == 0x01) && 	// number of block
		(req_packet_buff[4] == 0x01) &&		// quantity of blocks
		(req_packet_buff[5] == COMMAND_ID_INPUTS_STATE)
	) {

		// this is inputs state command

		if (req_packet_size != 8) return COMMUNICATION_COMMAND_PACKET_ERR; //wrong data in packets

		// this command works only for KDS
		if (get_device_type() != DEVICE_TYPE_KDS) return COMMUNICATION_COMMAND_MISMATCH;

		// prepare answer

		*ans_packet_buff++ = req_packet_buff[1]; // address of the recipient
		*ans_packet_buff++ = req_packet_buff[0]; // address of the sender

		*ans_packet_buff++ = TICKET_DATA_SEND; // packet type
		*ans_packet_buff++ = 0x01; // number of block
		*ans_packet_buff++ = 0x01; // quantity of blocks

		*ans_packet_buff++ = TICKET_ID_INPUTS_STATE;

		inputs = inputs_get_data(true);
		*ans_packet_buff++ = ptr[0]; // 0..7 in
		*ans_packet_buff = ptr[1]; // 8..15 in


		*ans_packet_size = 8;

		return COMMUNICATION_COMMAND_OK;

	}

	// this is not inputs state command

	return COMMUNICATION_COMMAND_MISMATCH;
}

enum COMMUNICATION_COMMAND_STATES communication_command_inputs_state_changed(
		uint8_t* req_packet_buff,
		uint8_t* ans_packet_buff,
		uint16_t req_packet_size,
		uint16_t* ans_packet_size
) {

	uint16_t data;
	uint8_t * ptr = (uint8_t*)&data;

	//check command

	if (
		(req_packet_buff[2] == COMMAND_DATA_REQUEST) && 	// packet type
		(req_packet_buff[3] == 0x01) && 	// number of block
		(req_packet_buff[4] == 0x01) &&		// quantity of blocks
		(req_packet_buff[5] == COMMAND_ID_INPUTS_STATE_CHANGED)
	) {

		// this is inputs state changed command

		if (req_packet_size != 8) return COMMUNICATION_COMMAND_PACKET_ERR; //wrong data in packets

		// this command works only for KDS
		if (get_device_type() != DEVICE_TYPE_KDS) return COMMUNICATION_COMMAND_MISMATCH;

		// prepare answer

		*ans_packet_buff++ = req_packet_buff[1]; // address of the recipient
		*ans_packet_buff++ = req_packet_buff[0]; // address of the sender

		*ans_packet_buff++ = TICKET_DATA_SEND; // packet type
		*ans_packet_buff++ = 0x01; // number of block
		*ans_packet_buff++ = 0x01; // quantity of blocks

		*ans_packet_buff++ = TICKET_ID_INPUTS_STATE_CHANGED;

		data = inputs_get_data(true);
		*ans_packet_buff++ = ptr[0]; // 0..7 in
		*ans_packet_buff++ = ptr[1]; // 8..15 in

		data = inputs_get_change();
		*ans_packet_buff++ = ptr[0]; // 0..7 in change flags
		*ans_packet_buff = ptr[1]; // 8..15 in change flags

		inputs_set_previous();

		*ans_packet_size = 10;

		return COMMUNICATION_COMMAND_OK;

	}

	// this is not inputs state changed command

	return COMMUNICATION_COMMAND_MISMATCH;
}

enum COMMUNICATION_COMMAND_STATES communication_command_inputs_state_blinking(
		uint8_t* req_packet_buff,
		uint8_t* ans_packet_buff,
		uint16_t req_packet_size,
		uint16_t* ans_packet_size
) {

	uint32_t data;
	uint8_t * ptr = (uint8_t*)&data;

	//check command

	if (
		(req_packet_buff[2] == COMMAND_DATA_REQUEST) && 	// packet type
		(req_packet_buff[3] == 0x01) && 	// number of block
		(req_packet_buff[4] == 0x01) &&		// quantity of blocks
		(req_packet_buff[5] == COMMAND_ID_INPUTS_STATE_BLINKING)
	) {

		// this is inputs state blinking command

		if (req_packet_size != 8) return COMMUNICATION_COMMAND_PACKET_ERR; //wrong data in packets

		// this command works only for KDS
		if (get_device_type() != DEVICE_TYPE_KDS) return COMMUNICATION_COMMAND_MISMATCH;

		// prepare answer

		*ans_packet_buff++ = req_packet_buff[1]; // address of the recipient
		*ans_packet_buff++ = req_packet_buff[0]; // address of the sender

		*ans_packet_buff++ = TICKET_DATA_SEND; // packet type
		*ans_packet_buff++ = 0x01; // number of block
		*ans_packet_buff++ = 0x01; // quantity of blocks

		*ans_packet_buff++ = TICKET_ID_INPUTS_STATE_BLINKING;

		data = inputs_get_concat_state_and_blinking();
		*ans_packet_buff++ = ptr[0]; // 0..3  in + blinking flags
		*ans_packet_buff++ = ptr[1]; // 4..7 in + blinking flags
		*ans_packet_buff++ = ptr[2]; // 8..11 in + blinking flags
		*ans_packet_buff = ptr[3]; // 11..15 in + blinking flags



		*ans_packet_size = 10;

		return COMMUNICATION_COMMAND_OK;

	}

	// this is not inputs state blinking command

	return COMMUNICATION_COMMAND_MISMATCH;
}

enum COMMUNICATION_COMMAND_STATES communication_command_outputs_preliminary(
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
		(req_packet_buff[5] == COMMAND_ID_OUTPUTS_PRELIMINARY)
	) {

		// this is outputs preliminary command

		if (req_packet_size != 10) return COMMUNICATION_COMMAND_PACKET_ERR; //wrong data in packets

		// this command works only for BTU
		if (get_device_type() != DEVICE_TYPE_BTU) return COMMUNICATION_COMMAND_MISMATCH;

		// prepare answer

		*ans_packet_buff++ = req_packet_buff[1]; // address of the recipient
		*ans_packet_buff++ = req_packet_buff[0]; // address of the sender

		*ans_packet_buff++ = TICKET_DATA_SEND; // packet type
		*ans_packet_buff++ = 0x01; // number of block
		*ans_packet_buff++ = 0x01; // quantity of blocks

		*ans_packet_buff++ = TICKET_ID_OUTPUTS_PRELIMINARY;

		*ans_packet_buff++ = 0x1;
		*ans_packet_buff = 0x1;


		*ans_packet_size = 8;

		return COMMUNICATION_COMMAND_OK;

	}

	// this is not outputs preliminary command

	return COMMUNICATION_COMMAND_MISMATCH;
}

enum COMMUNICATION_COMMAND_STATES communication_command_outputs_executive(
		uint8_t* req_packet_buff,
		uint8_t* ans_packet_buff,
		uint16_t req_packet_size,
		uint16_t* ans_packet_size
) {

	uint16_t duration;

	//check command

	if (
		(req_packet_buff[2] == COMMAND_DATA_REQUEST) && 	// packet type
		(req_packet_buff[3] == 0x01) && 	// number of block
		(req_packet_buff[4] == 0x01) &&		// quantity of blocks
		(req_packet_buff[5] == COMMAND_ID_OUTPUTS_EXECUTIVE)
	) {

		// this is outputs executive command

		if (req_packet_size != 12) return COMMUNICATION_COMMAND_PACKET_ERR; //wrong data in packets

		// this command works only for BTU
		if (get_device_type() != DEVICE_TYPE_BTU) return COMMUNICATION_COMMAND_MISMATCH;

		// prepare answer

		*ans_packet_buff++ = req_packet_buff[1]; // address of the recipient
		*ans_packet_buff++ = req_packet_buff[0]; // address of the sender

		*ans_packet_buff++ = TICKET_DATA_SEND; // packet type
		*ans_packet_buff++ = 0x01; // number of block
		*ans_packet_buff++ = 0x01; // quantity of blocks

		*ans_packet_buff++ = TICKET_ID_OUTPUTS_EXECUTIVE;

		*ans_packet_buff++ = 0x1;
		*ans_packet_buff = 0x1;

		duration = *((uint16_t*)&req_packet_buff[8]);

		//outputs_start_command(0, 100*(uint32_t)duration);
		outputs_start_command(0, 500, false);
		outputs_start_command(1, 1000, false);
		outputs_start_command(2, 1500, false);
		outputs_start_command(3, 2000, false);
		outputs_start_command(4, 2500, false);
		outputs_start_command(5, 3000, false);
		outputs_start_command(6, 3500, false);
		outputs_start_command(7, 4000, false);


		*ans_packet_size = 8;

		return COMMUNICATION_COMMAND_OK;

	}

	// this is not outputs executive command

	return COMMUNICATION_COMMAND_MISMATCH;
}

enum COMMUNICATION_COMMAND_STATES communication_command_work_time(
		uint8_t* req_packet_buff,
		uint8_t* ans_packet_buff,
		uint16_t req_packet_size,
		uint16_t* ans_packet_size
) {

	uint32_t wt = get_work_time_sek();
	uint8_t * ptr = (uint8_t*)&wt;

	//check command

	if (
		(req_packet_buff[2] == COMMAND_DATA_REQUEST) && 	// packet type
		(req_packet_buff[3] == 0x01) && 	// number of block
		(req_packet_buff[4] == 0x01) &&		// quantity of blocks
		(req_packet_buff[5] == COMMAND_ID_WORK_TIME)
	) {

		// this is work time command

		if (req_packet_size != 8) return COMMUNICATION_COMMAND_PACKET_ERR; //wrong data in packets

		// prepare answer

		*ans_packet_buff++ = req_packet_buff[1]; // address of the recipient
		*ans_packet_buff++ = req_packet_buff[0]; // address of the sender

		*ans_packet_buff++ = TICKET_DATA_SEND; // packet type
		*ans_packet_buff++ = 0x01; // number of block
		*ans_packet_buff++ = 0x01; // quantity of blocks

		*ans_packet_buff++ = TICKET_ID_WORK_TIME; //


		*ans_packet_buff++ = *ptr++;
		*ans_packet_buff++ = *ptr++;
		*ans_packet_buff++ = *ptr++;
		*ans_packet_buff   = *ptr;

		*ans_packet_size = 10;

		return COMMUNICATION_COMMAND_OK;

	}

	// this is not work time command

	return COMMUNICATION_COMMAND_MISMATCH;
}

enum COMMUNICATION_COMMAND_STATES communication_command_device_name(
		uint8_t* req_packet_buff,
		uint8_t* ans_packet_buff,
		uint16_t req_packet_size,
		uint16_t* ans_packet_size
) {

	uint8_t len;
	//check command

	if (
		(req_packet_buff[2] == COMMAND_DATA_REQUEST) && 	// packet type
		(req_packet_buff[3] == 0x01) && 	// number of block
		(req_packet_buff[4] == 0x01) &&		// quantity of blocks
		(req_packet_buff[5] == COMMAND_ID_DEVICE_NAME)
	) {

		// this is device name command

		if (req_packet_size != 8) return COMMUNICATION_COMMAND_PACKET_ERR; //wrong data in packets

		// prepare answer

		*ans_packet_buff++ = req_packet_buff[1]; // address of the recipient
		*ans_packet_buff++ = req_packet_buff[0]; // address of the sender

		*ans_packet_buff++ = TICKET_DATA_SEND; // packet type
		*ans_packet_buff++ = 0x01; // number of block
		*ans_packet_buff++ = 0x01; // quantity of blocks

		*ans_packet_buff++ = TICKET_ID_DEVICE_NAME; //

		len = copy_device_name(ans_packet_buff);

		ans_packet_buff[len] = 0x0; //end of string

		*ans_packet_size = 6 + len + 1;

		return COMMUNICATION_COMMAND_OK;

	}

	// this is not device name command

	return COMMUNICATION_COMMAND_MISMATCH;
}

enum COMMUNICATION_COMMAND_STATES communication_command_compilation_date(
		uint8_t* req_packet_buff,
		uint8_t* ans_packet_buff,
		uint16_t req_packet_size,
		uint16_t* ans_packet_size
		)  {

	uint8_t len;
	//check command

	if (
		(req_packet_buff[2] == COMMAND_DATA_REQUEST) && 	// packet type
		(req_packet_buff[3] == 0x01) && 	// number of block
		(req_packet_buff[4] == 0x01) &&		// quantity of blocks
		(req_packet_buff[5] == COMMAND_ID_COMPILATION_DATE)
	) {

		// this is compilation date command

		if (req_packet_size != 8) return COMMUNICATION_COMMAND_PACKET_ERR; //wrong data in packets

		// prepare answer

		*ans_packet_buff++ = req_packet_buff[1]; // address of the recipient
		*ans_packet_buff++ = req_packet_buff[0]; // address of the sender

		*ans_packet_buff++ = TICKET_DATA_SEND; // packet type
		*ans_packet_buff++ = 0x01; // number of block
		*ans_packet_buff++ = 0x01; // quantity of blocks

		*ans_packet_buff++ = TICKET_ID_COMPILATION_DATE; //

		len = copy_compilation_date(ans_packet_buff);

		ans_packet_buff[len] = 0x0; //end of string

		*ans_packet_size = 6 + len + 1;

		return COMMUNICATION_COMMAND_OK;

	}

	// this is not device name command

	return COMMUNICATION_COMMAND_MISMATCH;
}

enum COMMUNICATION_COMMAND_STATES communication_command_checksum(
		uint8_t* req_packet_buff,
		uint8_t* ans_packet_buff,
		uint16_t req_packet_size,
		uint16_t* ans_packet_size
		)  {

	uint8_t len;
	//check command

	if (
		(req_packet_buff[2] == COMMAND_DATA_REQUEST) && 	// packet type
		(req_packet_buff[3] == 0x01) && 	// number of block
		(req_packet_buff[4] == 0x01) &&		// quantity of blocks
		(req_packet_buff[5] == COMMAND_ID_CHECKSUM)
	) {

		// this is checksum command

		if (req_packet_size != 8) return COMMUNICATION_COMMAND_PACKET_ERR; //wrong data in packets

		// prepare answer

		*ans_packet_buff++ = req_packet_buff[1]; // address of the recipient
		*ans_packet_buff++ = req_packet_buff[0]; // address of the sender

		*ans_packet_buff++ = TICKET_DATA_SEND; // packet type
		*ans_packet_buff++ = 0x01; // number of block
		*ans_packet_buff++ = 0x01; // quantity of blocks

		*ans_packet_buff++ = TICKET_ID_CHECKSUM; //

		len = get_program_checksum(ans_packet_buff);

		*ans_packet_size = 6 + len;

		return COMMUNICATION_COMMAND_OK;

	}

	// this is not device name command

	return COMMUNICATION_COMMAND_MISMATCH;
}

enum COMMUNICATION_COMMAND_STATES communication_command_statistic(
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
		(req_packet_buff[5] == COMMAND_ID_STATISTIC)
	) {

		// this is statistic command

		if (req_packet_size != 8) return COMMUNICATION_COMMAND_PACKET_ERR; //wrong data in packets

		// prepare answer

		*ans_packet_buff++ = req_packet_buff[1]; // address of the recipient
		*ans_packet_buff++ = req_packet_buff[0]; // address of the sender

		*ans_packet_buff++ = TICKET_DATA_SEND; // packet type
		*ans_packet_buff++ = 0x01; // number of block
		*ans_packet_buff++ = 0x01; // quantity of blocks

		*ans_packet_buff++ = TICKET_ID_STATISTIC; //

		*ans_packet_size = 6 + communication_copy_statistic(ans_packet_buff);

		return COMMUNICATION_COMMAND_OK;

	}

	// this is not statistic command

	return COMMUNICATION_COMMAND_MISMATCH;
}

enum COMMUNICATION_COMMAND_STATES communication_command_write_parameters(
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
		(req_packet_buff[5] == COMMAND_ID_WRITE_PARAMETERS)
	) {

		// this is write parameters command

		if (req_packet_size != 20) return COMMUNICATION_COMMAND_PACKET_ERR; //wrong data in packets

		//check MAC
		if (!check_MAC((char*)&req_packet_buff[6])) return COMMUNICATION_COMMAND_PACKET_ERR; //wrong MAC

		set_serial_number(&req_packet_buff[12]);

		// prepare answer

		*ans_packet_buff++ = req_packet_buff[1]; // address of the recipient
		*ans_packet_buff++ = req_packet_buff[0]; // address of the sender

		*ans_packet_buff++ = TICKET_DATA_SEND; // packet type
		*ans_packet_buff++ = 0x01; // number of block
		*ans_packet_buff++ = 0x01; // quantity of blocks

		*ans_packet_buff++ = TICKET_ID_WRITE_PARAMETERS; //

		*ans_packet_size = 6;

		return COMMUNICATION_COMMAND_OK;

	}

	// this is not write parameters command
	return COMMUNICATION_COMMAND_MISMATCH;
}

enum COMMUNICATION_COMMAND_STATES communication_command_read_parameters(
		uint8_t* req_packet_buff,
		uint8_t* ans_packet_buff,
		uint16_t req_packet_size,
		uint16_t* ans_packet_size
		) {

	uint8_t size, mac_size;

	//check command

	if (
		(req_packet_buff[2] == COMMAND_DATA_REQUEST) && 	// packet type
		(req_packet_buff[3] == 0x01) && 	// number of block
		(req_packet_buff[4] == 0x01) &&		// quantity of blocks
		(req_packet_buff[5] == COMMAND_ID_READ_PARAMETERS)
	) {

		// this is read parameters command

		if (req_packet_size != 14) return COMMUNICATION_COMMAND_PACKET_ERR; //wrong data in packets

		//check MAC
		if (!check_MAC((char*)&req_packet_buff[6])) return COMMUNICATION_COMMAND_PACKET_ERR; //wrong MAC

		// prepare answer

		*ans_packet_buff++ = req_packet_buff[1]; // address of the recipient
		*ans_packet_buff++ = req_packet_buff[0]; // address of the sender

		*ans_packet_buff++ = TICKET_DATA_SEND; // packet type
		*ans_packet_buff++ = 0x01; // number of block
		*ans_packet_buff++ = 0x01; // quantity of blocks

		*ans_packet_buff++ = TICKET_ID_READ_PARAMETERS; //

		mac_size = copy_MAC((char *)ans_packet_buff); // MAC
		ans_packet_buff += mac_size;


		size = get_serial_number(ans_packet_buff);

		*ans_packet_size = 6 + mac_size + size;

		return COMMUNICATION_COMMAND_OK;

	}

	// this is not read parameters command
	return COMMUNICATION_COMMAND_MISMATCH;
}


enum COMMUNICATION_COMMAND_STATES communication_command_write_configuration(
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
		(req_packet_buff[5] == COMMAND_ID_WRITE_CONFIGURATION)
	) {

		// this is write configuration command

		if (req_packet_size != 30) return COMMUNICATION_COMMAND_PACKET_ERR; //wrong data in packets

		//check MAC
		if (!check_MAC((char*)&req_packet_buff[6])) return COMMUNICATION_COMMAND_PACKET_ERR; //wrong MAC

		set_configuration(&req_packet_buff[12]);

		// prepare answer

		*ans_packet_buff++ = req_packet_buff[1]; // address of the recipient
		*ans_packet_buff++ = req_packet_buff[0]; // address of the sender

		*ans_packet_buff++ = TICKET_DATA_SEND; // packet type
		*ans_packet_buff++ = 0x01; // number of block
		*ans_packet_buff++ = 0x01; // quantity of blocks

		*ans_packet_buff++ = TICKET_ID_WRITE_CONFIGURATION; //

		*ans_packet_size = 6;

		return COMMUNICATION_COMMAND_OK;

	}

	// this is not write configuration command
	return COMMUNICATION_COMMAND_MISMATCH;
}

enum COMMUNICATION_COMMAND_STATES communication_command_read_configuration(
		uint8_t* req_packet_buff,
		uint8_t* ans_packet_buff,
		uint16_t req_packet_size,
		uint16_t* ans_packet_size
		) {

	uint8_t size, mac_size;

	//check command

	if (
		(req_packet_buff[2] == COMMAND_DATA_REQUEST) && 	// packet type
		(req_packet_buff[3] == 0x01) && 	// number of block
		(req_packet_buff[4] == 0x01) &&		// quantity of blocks
		(req_packet_buff[5] == COMMAND_ID_READ_CONFIGURATION)
	) {

		// this is read configuration command

		if (req_packet_size != 14) return COMMUNICATION_COMMAND_PACKET_ERR; //wrong data in packets

		//check MAC
		if (!check_MAC((char*)&req_packet_buff[6])) return COMMUNICATION_COMMAND_PACKET_ERR; //wrong MAC

		// prepare answer

		*ans_packet_buff++ = req_packet_buff[1]; // address of the recipient
		*ans_packet_buff++ = req_packet_buff[0]; // address of the sender

		*ans_packet_buff++ = TICKET_DATA_SEND; // packet type
		*ans_packet_buff++ = 0x01; // number of block
		*ans_packet_buff++ = 0x01; // quantity of blocks

		*ans_packet_buff++ = TICKET_ID_READ_CONFIGURATION; //

		mac_size = copy_MAC((char *)ans_packet_buff); // MAC
		ans_packet_buff += mac_size;

		size = get_configuration(ans_packet_buff);

		*ans_packet_size = 6 + mac_size + size;

		return COMMUNICATION_COMMAND_OK;

	}

	// this is not read configuration command
	return COMMUNICATION_COMMAND_MISMATCH;
}

enum COMMUNICATION_STATES communication_commands_parser(
		uint8_t* req_packet_buff,
		uint8_t* ans_packet_buff,
		uint16_t req_packet_size,
		uint16_t* ans_packet_size
		) {

	enum COMMUNICATION_STATES state = COMMUNICATION_OK;
	enum COMMUNICATION_COMMAND_STATES func_pars_state = COMMUNICATION_COMMAND_OK;

	communication_commands_func * commands_table;
	uint16_t commands_table_size;
	enum DEVICE_TYPE type = get_device_type();


	if (type == DEVICE_TYPE_BTU) {
		commands_table = communication_commands_table_btu;
		commands_table_size = sizeof(communication_commands_table_btu)/sizeof(communication_commands_func);
	}
	else {
		commands_table = communication_commands_table_kds;
		commands_table_size = sizeof(communication_commands_table_kds)/sizeof(communication_commands_func);
	}


	for (uint16_t i=0; i<commands_table_size; ++i) {
		func_pars_state = commands_table[i](req_packet_buff, ans_packet_buff, req_packet_size, ans_packet_size);
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
				//
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
	*ans_packet_buff++ = (req_packet_buff[0]==0xFF) ? 0xFF : req_packet_buff[0]; // address of the sender

	*ans_packet_buff++ = ticket; // packet type


	*ans_packet_size = 3;

	return COMMUNICATION_OK;
}
