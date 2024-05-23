/*
 * test.h
 *
 *  Created on: May 16, 2024
 *      Author: lepikhov
 */

#ifndef INC_INPUTS_H_
#define INC_INPUTS_H_


#define INPUT_DATA_FILTER_CAPACITY 10
#define INPUT_DATA_FILTER_THRESHOLD 7

struct inputs_t {
	uint16_t current_data;
	uint16_t previous_data;
	uint16_t changed_data;
	uint8_t  data_sum[16];
	uint8_t  data_counter;
};

void inputs_func(void);
void inputs_check_data_func(void);
uint16_t inputs_get_data(bool invers);
uint16_t inputs_get_change(void);
void inputs_set_previous(void);

#endif /* INC_INPUTS_H_ */
