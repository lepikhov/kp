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
	uint8_t states[8];
	bool commands[8];
	uint32_t start_ticks[8];
	uint32_t durations[8];

	bool is_tmr1_dma_run;
	bool is_tmr3_dma_run;

	uint32_t tmr1_dma_buff[OUTPUTS_DMA_BUFFER_SIZE];
	uint32_t tmr3_dma_buff[OUTPUTS_DMA_BUFFER_SIZE];
};


#define CHECK_OUTPUT_HALF_PULSE_DURATION 10 //10 mlsek
#define CHECK_OUTPUT_IDLE_DURATION 5000 //5000 mlsek

struct check_outputs_t {
	uint8_t phase;
	uint32_t start_tick;
	uint8_t errors[8];
};


void set_dir_in(GPIO_TypeDef * gpio_base, uint8_t bit);
void set_dir_out(GPIO_TypeDef * gpio_base, uint8_t bit);

void outputs_start_command(uint8_t command, uint32_t duration, bool is_check);
void outputs_func(void);
bool outputs_check_preliminary_commands(uint8_t outputs1, uint8_t outputs2);
uint8_t outputs_get_commands(void);
void outputs_check_func(void);
uint8_t outputs_get_errors(void);



#endif /* INC_OUTPUTS_H_ */
