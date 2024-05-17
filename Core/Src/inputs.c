/*
 * test.c
 *
 *  Created on: May 16, 2024
 *      Author: lepikhov
 */


#include <stdint.h>
#include <stdbool.h>
#include "main.h"
#include <inputs.h>

uint16_t inputs_data=0;


void inputs_func(void) {
	inputs_check_data_func();
}

uint16_t inputs_get_data(bool invers) {
	return invers ? ~inputs_data : inputs_data;
}

void inputs_check_data_func(void)
{
	if( HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_1) == GPIO_PIN_SET ) {
	    inputs_data |= 0x1;
	}
	else {
	    inputs_data &= ~0x1;
	}

	if( HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_2) == GPIO_PIN_SET ) {
	    inputs_data |= 0x2;
	}
	else {
	    inputs_data &= ~0x2;
	}

	if( HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_3) == GPIO_PIN_SET ) {
	    inputs_data |= 0x4;
	}
	else {
	    inputs_data &= ~0x4;
	}

	if( HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_4) == GPIO_PIN_SET ) {
	    inputs_data |= 0x8;
	}
	else {
	    inputs_data &= ~0x8;
	}

	if( HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_5) == GPIO_PIN_SET ) {
	    inputs_data |= 0x10;
	}
	else {
	    inputs_data &= ~0x10;
	}

	if( HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4) == GPIO_PIN_SET ) {
	    inputs_data |= 0x20;
	}
	else {
	    inputs_data &= ~0x20;
	}

	if( HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5) == GPIO_PIN_SET ) {
	    inputs_data |= 0x40;
	}
	else {
	    inputs_data &= ~0x40;
	}

	if( HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6) == GPIO_PIN_SET ) {
	    inputs_data |= 0x80;
	}
	else {
	    inputs_data &= ~0x80;
	}

	if( HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7) == GPIO_PIN_SET ) {
	    inputs_data |= 0x0100;
	}
	else {
	    inputs_data &= ~0x0100;
	}

	if( HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6) == GPIO_PIN_SET ) {
	    inputs_data |= 0x0200;
	}
	else {
	    inputs_data &= ~0x0200;
	}

	if( HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5) == GPIO_PIN_SET ) {
	    inputs_data |= 0x0400;
	}
	else {
	    inputs_data &= ~0x0400;
	}

	if( HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_2) == GPIO_PIN_SET ) {
	    inputs_data |= 0x0800;
	}
	else {
	    inputs_data &= ~0x0800;
	}

	if( HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_12) == GPIO_PIN_SET ) {
	    inputs_data |= 0x1000;
	}
	else {
	    inputs_data &= ~0x1000;
	}

	if( HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_11) == GPIO_PIN_SET ) {
	    inputs_data |= 0x2000;
	}
	else {
	    inputs_data &= ~0x2000;
	}

	if( HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_10) == GPIO_PIN_SET ) {
	    inputs_data |= 0x4000;
	}
	else {
	    inputs_data &= ~0x4000;
	}

	if( HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_10) == GPIO_PIN_SET ) {
	    inputs_data |= 0x8000;
	}
	else {
	    inputs_data &= ~0x8000;
	}

}
