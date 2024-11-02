/*
 * inputs.c
 *
 *  Created on: May 16, 2024
 *      Author: lepikhov
 */


#include <stdint.h>
#include <stdbool.h>
#include "main.h"
#include <inputs.h>
#include <device.h>

struct inputs_t inputs={0};
struct inputs_blinking_t inputs_blinking={0};


void inputs_func(void) {
	inputs_check_data_func(false);
}

uint16_t inputs_get_data(bool invers) {
	return invers ? ~inputs.current_data : inputs.current_data;
}


void inputs_set_previous(void) {
	inputs.previous_data = inputs.current_data;
}

uint16_t inputs_get_change(void) {
	inputs.changed_data = inputs.previous_data ^ inputs.current_data;
	return inputs.changed_data;
}

void inputs_check_data_func(bool out_control) {

	uint8_t filter_capacity, filter_threshold;

	if( HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7) == GPIO_PIN_SET ) ++inputs.data_sum[0];
	if( HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6) == GPIO_PIN_SET ) ++inputs.data_sum[1];
	if( HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5) == GPIO_PIN_SET ) ++inputs.data_sum[2];
	if( HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_2) == GPIO_PIN_SET ) ++inputs.data_sum[3];
	if( HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_12) == GPIO_PIN_SET ) ++inputs.data_sum[4];
	if( HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_11) == GPIO_PIN_SET ) ++inputs.data_sum[5];
	if( HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_10) == GPIO_PIN_SET ) ++inputs.data_sum[6];
	if( HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_10) == GPIO_PIN_SET ) ++inputs.data_sum[7];

	if (!out_control) {
		if( HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_1) == GPIO_PIN_SET ) ++inputs.data_sum[8];
		if( HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_2) == GPIO_PIN_SET ) ++inputs.data_sum[9];
		if( HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_3) == GPIO_PIN_SET ) ++inputs.data_sum[10];
		if( HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_4) == GPIO_PIN_SET ) ++inputs.data_sum[11];
		if( HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_5) == GPIO_PIN_SET ) ++inputs.data_sum[12];
		if( HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4) == GPIO_PIN_SET ) ++inputs.data_sum[13];
		if( HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5) == GPIO_PIN_SET ) ++inputs.data_sum[14];
		if( HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6) == GPIO_PIN_SET ) ++inputs.data_sum[15];
	}

	filter_capacity = out_control
			?
			INPUT_DATA_FILTER_CAPACITY_OUT_CONTROL
			:
			INPUT_DATA_FILTER_CAPACITY;

	filter_threshold = out_control
			?
			INPUT_DATA_FILTER_THRESHOLD_OUT_CONTROL
			:
			INPUT_DATA_FILTER_THRESHOLD;

	if (++inputs.data_counter > filter_capacity) {
		uint16_t in = 0x1;

		inputs.current_data = 0;
		for (uint8_t i=0; i<16; ++i) {

			if (out_control && (i>7) ) break;

			if (inputs.data_sum[i] > filter_threshold) inputs.current_data |= in;
			inputs.data_sum[i] = 0;

			in <<= 1;
		}

		inputs.data_counter = 0;
	}

	if (!out_control) inputs_check_blinking();

}




void inputs_check_blinking(void) {

	uint16_t mask = 0x1;
	bool is_check_interval = false;

	if (get_delta_tick(inputs_blinking.start_tick) >= INPUTS_BLINKING_CHECK_INTERVAL) {
		inputs_blinking.start_tick = HAL_GetTick();
		is_check_interval = true;
	}

	inputs_blinking.current_data = inputs_get_data(true);


	for (uint8_t i=0; i<16; ++i) {
		if ( mask & (inputs_blinking.current_data ^ inputs_blinking.previous_data) ) {
			++inputs_blinking.counters[i];
		}

		if (is_check_interval) {
			if (inputs_blinking.counters[i] > INPUTS_BLINKING_CHECK_THRESHOLD) {
				inputs_blinking.blinking |= mask;
			}
			else {
				inputs_blinking.blinking &= ~mask;
			}
			inputs_blinking.counters[i] = 0;
		}

		mask <<= 1;
	}

	inputs_blinking.previous_data = inputs_blinking.current_data;
}


uint16_t inputs_get_blinking(void) {
	return inputs_blinking.blinking;
}

uint32_t inputs_get_concat_state_and_blinking(void) {
	uint32_t concat=0;
	uint16_t state = inputs_get_data(true);
	uint16_t blinking = inputs_get_blinking();
	uint16_t mask1 = 0x1;
	uint32_t mask2 = 0x1;

	for (uint8_t i=0; i<16; ++i) {
		if (mask1 & state) concat |= mask2;
		mask2 <<= 1;
		if (mask1 & blinking) concat |= mask2;
		mask2 <<= 1;
		mask1 <<= 1;
	}

	return concat;

}

