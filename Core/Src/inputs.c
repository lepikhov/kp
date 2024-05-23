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

struct inputs_t inputs={0};


void inputs_func(void) {
	inputs_check_data_func();
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

void inputs_check_data_func(void)
{

	if( HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7) == GPIO_PIN_SET ) ++inputs.data_sum[0];
	if( HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6) == GPIO_PIN_SET ) ++inputs.data_sum[1];
	if( HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5) == GPIO_PIN_SET ) ++inputs.data_sum[2];
	if( HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_2) == GPIO_PIN_SET ) ++inputs.data_sum[3];
	if( HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_12) == GPIO_PIN_SET ) ++inputs.data_sum[4];
	if( HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_11) == GPIO_PIN_SET ) ++inputs.data_sum[5];
	if( HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_10) == GPIO_PIN_SET ) ++inputs.data_sum[6];
	if( HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_10) == GPIO_PIN_SET ) ++inputs.data_sum[7];
	if( HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_1) == GPIO_PIN_SET ) ++inputs.data_sum[8];
	if( HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_2) == GPIO_PIN_SET ) ++inputs.data_sum[9];
	if( HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_3) == GPIO_PIN_SET ) ++inputs.data_sum[10];
	if( HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_4) == GPIO_PIN_SET ) ++inputs.data_sum[11];
	if( HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_5) == GPIO_PIN_SET ) ++inputs.data_sum[12];
	if( HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4) == GPIO_PIN_SET ) ++inputs.data_sum[13];
	if( HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5) == GPIO_PIN_SET ) ++inputs.data_sum[14];
	if( HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6) == GPIO_PIN_SET ) ++inputs.data_sum[15];

	if (++inputs.data_counter > INPUT_DATA_FILTER_CAPACITY) {
		uint16_t in = 0x1;

		inputs.current_data = 0;
		for (uint8_t i=0; i<16; ++i) {
			if (inputs.data_sum[i] > INPUT_DATA_FILTER_THRESHOLD) inputs.current_data |= in;
			inputs.data_sum[i] = 0;

			in <<= 1;
		}

		inputs.data_counter = 0;
	}

}
