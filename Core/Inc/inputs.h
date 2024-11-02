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

#define INPUT_DATA_FILTER_CAPACITY_OUT_CONTROL 10
#define INPUT_DATA_FILTER_THRESHOLD_OUT_CONTROL 7

struct inputs_t {
	uint16_t current_data;
	uint16_t previous_data;
	uint16_t changed_data;
	uint8_t  data_sum[16];
	uint8_t  data_counter;
};

#define INPUTS_BLINKING_CHECK_INTERVAL 5000 // 5 sek
#define INPUTS_BLINKING_CHECK_THRESHOLD 2 // two changes in 5 sek

struct inputs_blinking_t {
	uint16_t blinking;

	uint16_t current_data;
	uint16_t previous_data;
	uint16_t  counters[16];
	uint32_t start_tick;
};

void inputs_func(void);
void inputs_check_data_func(bool out_control);
uint16_t inputs_get_data(bool invers);
uint16_t inputs_get_change(void);
void inputs_set_previous(void);

void inputs_check_blinking(void);
uint16_t inputs_get_blinking(void);
uint32_t inputs_get_concat_state_and_blinking(void);

#endif /* INC_INPUTS_H_ */
