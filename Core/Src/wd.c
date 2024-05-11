/*
 * wd.c
 *
 *  Created on: Nov 29, 2023
 *      Author: lepikhov
 */
#include "main.h"
#include "wd.h"

void wd_init()
{

}

void ext_wd_func()
{
	HAL_GPIO_TogglePin (GPIOC, GPIO_PIN_0);
}
