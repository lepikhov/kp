/*
 * commands.h
 *
 *  Created on: May 8, 2024
 *      Author: Pavel
 */

#ifndef INC_COMMUNICATION_COMMANDS_H_
#define INC_COMMUNICATION_COMMANDS_H_

enum COMMUNICATION_STATES communication_commands_parser(
		uint8_t* req_packet_buff,
		uint8_t* ans_packet_buff,
		uint16_t req_packet_size,
		uint16_t* ans_packet_size
		);

typedef enum COMMUNICATION_COMMAND_STATES (*communication_commands_func)(
		uint8_t* req_packet_buff,
		uint8_t* ans_packet_buff,
		uint16_t req_packet_size,
		uint16_t* ans_packet_size
		);

enum COMMUNICATION_COMMAND_STATES communication_command_identification(
		uint8_t* req_packet_buff,
		uint8_t* ans_packet_buff,
		uint16_t req_packet_size,
		uint16_t* ans_packet_size
		);

#endif /* INC_COMMUNICATION_COMMANDS_H_ */
