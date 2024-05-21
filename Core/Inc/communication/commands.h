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
#define COMMAND_DATA_REQUEST	0xA0

//COMMANDS IDS
#define COMMAND_ID_INPUTS_STATE		0x00
#define COMMAND_ID_DEVICE_NAME		0x22
#define COMMAND_ID_WORK_TIME 		0x32
#define COMMAND_ID_MAC			   	0x41

//ANSWER TICKETS
#define TICKET_IDENTIFICATION	  0x00
#define TICKET_DATA_SEND		  0x20
#define TICKET_WRONG_COMMAND  	  0x21
#define TICKET_WRONG_PACKET_DATA  0x22

//ANSWER TICKETS IDS
#define TICKET_ID_DATA			0x00
#define TICKET_ID_DEVICE_NAME	0x21
#define TICKET_ID_WORK_TIME 	0x32
#define TICKET_ID_MAC			0x41

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

enum COMMUNICATION_COMMAND_STATES communication_command_inputs_state(
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

#endif /* INC_COMMUNICATION_COMMANDS_H_ */
