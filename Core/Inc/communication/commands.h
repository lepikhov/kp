/*
 * commands.h
 *
 *  Created on: May 8, 2024
 *      Author: Pavel
 */

#ifndef INC_COMMUNICATION_COMMANDS_H_
#define INC_COMMUNICATION_COMMANDS_H_


//COMMANDS TYPES
#define COMMAND_IDENTIFICATION 	0x80
#define COMMAND_RESET		 	0x81
#define COMMAND_DATA_REQUEST	0xA0

//COMMANDS IDS
#define COMMAND_ID_INPUTS_STATE				0x00
#define COMMAND_ID_INPUTS_STATE_CHANGED		0x01
#define COMMAND_ID_INPUTS_STATE_BLINKING	0x02
#define COMMAND_ID_COMPILATION_DATE 		0x21
#define COMMAND_ID_DEVICE_NAME				0x22
#define COMMAND_ID_STATISTIC			   	0x31
#define COMMAND_ID_WORK_TIME 				0x32
#define COMMAND_ID_MAC			   			0x41
#define COMMAND_ID_READ_CONFIGURATION		0x42
#define COMMAND_ID_WRITE_CONFIGURATION		0x43
#define COMMAND_ID_READ_PARAMETERS			0x44
#define COMMAND_ID_WRITE_PARAMETERS			0x45

//ANSWER TICKETS
#define TICKET_IDENTIFICATION	  0x00
#define TICKET_DATA_SEND		  0x20
#define TICKET_WRONG_COMMAND  	  0x21
#define TICKET_WRONG_PACKET_DATA  0x22
#define TICKET_RESET			  0x23

//ANSWER TICKETS IDS
#define TICKET_ID_DATA								0x00
#define TICKET_ID_INPUTS_STATE						0x00
#define TICKET_ID_INPUTS_STATE_CHANGED				0x01
#define TICKET_ID_INPUTS_STATE_BLINKING				0x02
#define TICKET_ID_COMPILATION_DATE 					0x21
#define TICKET_ID_DEVICE_NAME						0x22
#define TICKET_ID_STATISTIC							0x31
#define TICKET_ID_WORK_TIME 						0x32
#define TICKET_ID_MAC								0x41
#define TICKET_ID_READ_CONFIGURATION				0x42
#define TICKET_ID_WRITE_CONFIGURATION				0x43
#define TICKET_ID_READ_PARAMETERS					0x44
#define TICKET_ID_WRITE_PARAMETERS					0x45

#define DEVICE_MANUFACTURE_ID 	0x37
#define DEVICE_TYPE_KDS_ID		0x10
#define DEVICE_TYPE_BTU_ID		0x80
#define DEVICE_TYPE_UNKNOWN_ID	0xFF

typedef enum COMMUNICATION_COMMAND_STATES (*communication_commands_func)(
		uint8_t* req_packet_buff,
		uint8_t* ans_packet_buff,
		uint16_t req_packet_size,
		uint16_t* ans_packet_size
		);


enum COMMUNICATION_STATES communication_commands_parser(
		uint8_t* req_packet_buff,
		uint8_t* ans_packet_buff,
		uint16_t req_packet_size,
		uint16_t* ans_packet_size
		);

enum COMMUNICATION_COMMAND_STATES communication_command_error_response(
		uint8_t ticket,
		uint8_t* req_packet_buff,
		uint8_t* ans_packet_buff,
		uint16_t* ans_packet_size
		);


//COMMANDS

enum COMMUNICATION_COMMAND_STATES communication_command_identification(
		uint8_t* req_packet_buff,
		uint8_t* ans_packet_buff,
		uint16_t req_packet_size,
		uint16_t* ans_packet_size
		);

enum COMMUNICATION_COMMAND_STATES communication_command_MAC(
		uint8_t* req_packet_buff,
		uint8_t* ans_packet_buff,
		uint16_t req_packet_size,
		uint16_t* ans_packet_size
		);

enum COMMUNICATION_COMMAND_STATES communication_command_reset(
		uint8_t* req_packet_buff,
		uint8_t* ans_packet_buff,
		uint16_t req_packet_size,
		uint16_t* ans_packet_size
		);

enum COMMUNICATION_COMMAND_STATES communication_command_inputs_state(
		uint8_t* req_packet_buff,
		uint8_t* ans_packet_buff,
		uint16_t req_packet_size,
		uint16_t* ans_packet_size
		);

enum COMMUNICATION_COMMAND_STATES communication_command_inputs_state_changed(
		uint8_t* req_packet_buff,
		uint8_t* ans_packet_buff,
		uint16_t req_packet_size,
		uint16_t* ans_packet_size
		);

enum COMMUNICATION_COMMAND_STATES communication_command_inputs_state_blinking(
		uint8_t* req_packet_buff,
		uint8_t* ans_packet_buff,
		uint16_t req_packet_size,
		uint16_t* ans_packet_size
		);

enum COMMUNICATION_COMMAND_STATES communication_command_work_time(
		uint8_t* req_packet_buff,
		uint8_t* ans_packet_buff,
		uint16_t req_packet_size,
		uint16_t* ans_packet_size
		);

enum COMMUNICATION_COMMAND_STATES communication_command_device_name(
		uint8_t* req_packet_buff,
		uint8_t* ans_packet_buff,
		uint16_t req_packet_size,
		uint16_t* ans_packet_size
		);

enum COMMUNICATION_COMMAND_STATES communication_command_compilation_date(
		uint8_t* req_packet_buff,
		uint8_t* ans_packet_buff,
		uint16_t req_packet_size,
		uint16_t* ans_packet_size
		);

enum COMMUNICATION_COMMAND_STATES communication_command_statistic(
		uint8_t* req_packet_buff,
		uint8_t* ans_packet_buff,
		uint16_t req_packet_size,
		uint16_t* ans_packet_size
		);

enum COMMUNICATION_COMMAND_STATES communication_command_write_parameters(
		uint8_t* req_packet_buff,
		uint8_t* ans_packet_buff,
		uint16_t req_packet_size,
		uint16_t* ans_packet_size
		);

enum COMMUNICATION_COMMAND_STATES communication_command_read_parameters(
		uint8_t* req_packet_buff,
		uint8_t* ans_packet_buff,
		uint16_t req_packet_size,
		uint16_t* ans_packet_size
		);

enum COMMUNICATION_COMMAND_STATES communication_command_write_configuration(
		uint8_t* req_packet_buff,
		uint8_t* ans_packet_buff,
		uint16_t req_packet_size,
		uint16_t* ans_packet_size
		);

enum COMMUNICATION_COMMAND_STATES communication_command_read_configuration(
		uint8_t* req_packet_buff,
		uint8_t* ans_packet_buff,
		uint16_t req_packet_size,
		uint16_t* ans_packet_size
		);

#endif /* INC_COMMUNICATION_COMMANDS_H_ */
