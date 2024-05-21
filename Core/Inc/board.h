/*
 * board.h
 *
 *  Created on: Apr 23, 2024
 *      Author: lepikhov
 */

#ifndef INC_BOARD_H_
#define INC_BOARD_H_

#define RS485_DIR_TX HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET)
#define RS485_DIR_RX HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET)

#define TOGGLE_LED1 HAL_GPIO_TogglePin (GPIOB, GPIO_PIN_0)
#define TOGGLE_LED2 HAL_GPIO_TogglePin (GPIOA, GPIO_PIN_7)

#endif /* INC_BOARD_H_ */
