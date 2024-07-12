/*
 * outputs.h
 *
 *  Created on: Jul 1, 2024
 *      Author: lepikhov
 */

#ifndef INC_OUTPUTS_H_
#define INC_OUTPUTS_H_

#define MIN_COMMAND_DURATION 100 //100 mlsek

#define OUTPUTS_DMA_BUFFER_SIZE 5

struct outputs_dma_t {
	uint32_t buff[OUTPUTS_DMA_BUFFER_SIZE];
	uint32_t bsrr;
	uint32_t gpio_base;
	uint8_t  bit;
};

struct outputs_t {
	uint8_t states[8+2];
	bool commands[8+2];
	uint32_t start_ticks[8+2];
	uint32_t durations[8+2];
};


void set_dir_in(GPIO_TypeDef * gpio_base, uint8_t bit);
void set_dir_out(GPIO_TypeDef * gpio_base, uint8_t bit);

void outputs_start_command(uint8_t command, uint32_t duration);
void outputs_func(void);
bool outputs_check_preliminary_commands(uint8_t outputs1, uint8_t outputs2);
uint8_t outputs_get_commands(void);



#endif /* INC_OUTPUTS_H_ */
