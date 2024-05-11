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

#endif /* INC_BOARD_H_ */
