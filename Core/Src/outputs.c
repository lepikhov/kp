/*
 * outputs.c
 *
 *  Created on: Jul 1, 2024
 *      Author: lepikhov
 */


#include <stdint.h>
#include <stdbool.h>
#include "main.h"
#include <inputs.h>
#include <outputs.h>
#include <device.h>

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim3;

const struct outputs_dma_t outputs_dma[] = {
// Data_0
{
		.buff = {0x00000002, 0x00020000, 0x00020000, 0x00020000, 0x00020000},
		.bsrr = (uint32_t)&GPIOC->BSRR,
		.gpio_base = (uint32_t)GPIOC,
		.bit = 1
},
// Data_1
{
		.buff = {0x00000004, 0x00040000, 0x00040000, 0x00040000, 0x00040000},
		.bsrr = (uint32_t)&GPIOC->BSRR,
		.gpio_base = (uint32_t)GPIOC,
		.bit = 2
},
// Data_2
{
		.buff = {0x00000008, 0x00080000, 0x00080000, 0x00080000, 0x00080000},
		.bsrr = (uint32_t)&GPIOC->BSRR,
		.gpio_base = (uint32_t)GPIOC,
		.bit = 3
},
// Data_3
{
		.buff = {0x00000010, 0x00100000, 0x00100000, 0x00100000, 0x00100000},
		.bsrr = (uint32_t)&GPIOC->BSRR,
		.gpio_base = (uint32_t)GPIOC,
		.bit = 4
},
// Data_4
{
		.buff = {0x00000020, 0x00200000, 0x00200000, 0x00200000, 0x00200000},
		.bsrr = (uint32_t)&GPIOC->BSRR,
		.gpio_base = (uint32_t)GPIOC,
		.bit = 5
},
// Data_5
{
		.buff = {0x00000010, 0x00100000, 0x00100000, 0x00100000, 0x00100000},
		.bsrr = (uint32_t)&GPIOA->BSRR,
		.gpio_base = (uint32_t)GPIOA,
		.bit = 4
},
// Data_6
{
		.buff = {0x00000020, 0x00200000, 0x00200000, 0x00200000, 0x00200000},
		.bsrr = (uint32_t)&GPIOA->BSRR,
		.gpio_base = (uint32_t)GPIOA,
		.bit = 5
},
// Data_7
{
		.buff = {0x00000040, 0x00400000, 0x00400000, 0x00400000, 0x00400000},
		.bsrr = (uint32_t)&GPIOA->BSRR,
		.gpio_base = (uint32_t)GPIOA,
		.bit = 6
},
// VD2
{
		.buff = {0x00000080, 0x00800000, 0x00800000, 0x00800000, 0x00800000},
		.bsrr = (uint32_t)&GPIOA->BSRR,
		.gpio_base = (uint32_t)GPIOA,
		.bit = 7
},
// VD1
{
		.buff = {0x00000001, 0x00010000, 0x00010000, 0x00010000, 0x00010000},
		.bsrr = (uint32_t)&GPIOB->BSRR,
		.gpio_base = (uint32_t)GPIOB,
		.bit = 0
},
};

struct outputs_t outputs={0};

struct check_outputs_t check_outputs={0};

void set_dir_in(GPIO_TypeDef * gpio_base, uint8_t bit) {
	bit &= 0xFF;

	if (bit < 8) {
		gpio_base->CRL &= ~((uint32_t)(0x0000000f) << (bit*4));
		gpio_base->CRL |= (uint32_t)(0x00000000) << (bit*4);
	}
	else {
		bit -= 4;
		gpio_base->CRH &= ~((uint32_t)(0x0000000f) << (bit*4));
		gpio_base->CRH |= (uint32_t)(0x00000000) << (bit*4);
	}
}

void set_dir_out(GPIO_TypeDef * gpio_base, uint8_t bit) {
	bit &= 0xFF;

	if (bit < 8) {
		gpio_base->CRL &= ~((uint32_t)(0x0000000f) << (bit*4));
		gpio_base->CRL |= (uint32_t)(0x00000002) << (bit*4);
	}
	else {
		bit -= 4;
		gpio_base->CRH &= ~((uint32_t)(0x0000000f) << (bit*4));
		gpio_base->CRH |= (uint32_t)(0x00000002) << (bit*4);
	}
}


void outputs_start_command(uint8_t command, uint32_t duration, bool is_check) {

	if (command > 7) return;

	set_dir_out((GPIO_TypeDef *)outputs_dma[command].gpio_base, outputs_dma[command].bit);

	outputs.settings[command] = true;
	outputs.durations[command] = is_check
			?
			duration
			:
			(duration > MIN_COMMAND_DURATION) ? duration : MIN_COMMAND_DURATION;

	outputs.start_ticks[command] = HAL_GetTick();


	if (command < 5) {

		for (uint8_t i=0; i<OUTPUTS_DMA_BUFFER_SIZE; ++i) {
			outputs.tmr1_dma_buff[i] |= outputs_dma[command].buff[i];
		}

		if (!outputs.is_tmr1_dma_run) {

			outputs.is_tmr1_dma_run = true;

			HAL_DMA_Start_IT(htim1.hdma[TIM_DMA_ID_UPDATE],
					(uint32_t)outputs.tmr1_dma_buff,
					(uint32_t)outputs_dma[command].bsrr,
					OUTPUTS_DMA_BUFFER_SIZE);

			__HAL_TIM_ENABLE_DMA(&htim1, TIM_DMA_UPDATE);
			__HAL_TIM_ENABLE(&htim1);
		}
	}
	else {

		for (uint8_t i=0; i<OUTPUTS_DMA_BUFFER_SIZE; ++i) {
			outputs.tmr3_dma_buff[i] |= outputs_dma[command].buff[i];
		}

		if (!outputs.is_tmr3_dma_run) {

			outputs.is_tmr3_dma_run = true;

			HAL_DMA_Start_IT(htim3.hdma[TIM_DMA_ID_UPDATE],
					(uint32_t)outputs.tmr3_dma_buff,
					(uint32_t)outputs_dma[command].bsrr,
					OUTPUTS_DMA_BUFFER_SIZE);

			__HAL_TIM_ENABLE_DMA(&htim3, TIM_DMA_UPDATE);
			__HAL_TIM_ENABLE(&htim3);
		}
	}


}

void outputs_func(void) {
	bool execute = false;
	for (uint8_t i=0; i<8; ++i) {
		if (outputs.settings[i]) {
			execute = true;
			if (get_delta_tick(outputs.start_ticks[i]) >= outputs.durations[i]) {

				outputs.settings[i] = false;

				if (i<5) {

					for (uint8_t j=0; j<OUTPUTS_DMA_BUFFER_SIZE; ++j) {
						outputs.tmr1_dma_buff[j] &= ~outputs_dma[i].buff[j];
					}

					if (!outputs.tmr1_dma_buff[0]) {

						outputs.is_tmr1_dma_run = false;

						__HAL_TIM_DISABLE(&htim1);
						__HAL_TIM_DISABLE_DMA(&htim1, TIM_DMA_UPDATE);
						HAL_DMA_Abort_IT(htim1.hdma[TIM_DMA_ID_UPDATE]);
					}
				}
				else {

					for (uint8_t j=0; j<OUTPUTS_DMA_BUFFER_SIZE; ++j) {
						outputs.tmr3_dma_buff[j] &= ~outputs_dma[i].buff[j];
					}

					if (!outputs.tmr3_dma_buff[0]) {

						outputs.is_tmr3_dma_run = false;

						__HAL_TIM_DISABLE(&htim3);
						__HAL_TIM_DISABLE_DMA(&htim3, TIM_DMA_UPDATE);
						HAL_DMA_Abort_IT(htim3.hdma[TIM_DMA_ID_UPDATE]);
					}
				}

				((GPIO_TypeDef *)outputs_dma[i].gpio_base)->BSRR = outputs_dma[i].buff[1];
				set_dir_in((GPIO_TypeDef *)outputs_dma[i].gpio_base, outputs_dma[i].bit);
			}
		}
	}

	if (!execute && (outputs.command_phase==OUTPUTS_PHASE_EXECUTE)) {
		outputs.command_phase=OUTPUTS_PHASE_IDLE;
		outputs_set_command_code(0x00FF);
	}

	inputs_check_data_func(true);

}

bool outputs_check_command_code(uint16_t command_code) {
	uint8_t o1 = ((uint8_t*)&command_code)[0];
	uint8_t o2 = ((uint8_t*)&command_code)[1];

	return ((o1 ^ o2) == 0xff);
}

uint8_t outputs_get_settings(void) {
	uint8_t settings = 0;
	uint8_t mask = 0x1;
	for (uint8_t i=0; i<8; ++i) {
		if (outputs.settings[i]) settings |= mask;
		mask <<= 1;
	}
	return settings;

}

void outputs_check_func(void) {

	if (outputs.command_phase == OUTPUTS_PHASE_EXECUTE) {
		check_outputs.phase = 0;
		return;
	}

	if (!check_outputs.phase) {
		check_outputs.start_tick = HAL_GetTick();
		if (!outputs.settings[check_outputs.phase]) {
			outputs_start_command(0, 2*CHECK_OUTPUT_HALF_PULSE_DURATION, true);
		}
		++check_outputs.phase;
		return;
	}

	if (check_outputs.phase<8) {
		if (get_delta_tick(check_outputs.start_tick) >= 2*CHECK_OUTPUT_HALF_PULSE_DURATION) {
			if (!outputs.settings[check_outputs.phase]) {
				outputs_start_command(check_outputs.phase, 2*CHECK_OUTPUT_HALF_PULSE_DURATION, true);
			}
			check_outputs.start_tick = HAL_GetTick();
			++check_outputs.phase;
		}
		else {
			if (get_delta_tick(check_outputs.start_tick) >= CHECK_OUTPUT_HALF_PULSE_DURATION) {
				uint8_t ins = inputs_get_data(true)&0xff;
				for (uint8_t i=0; i<8; ++i) {
					bool state = ins&(1<<i);
					if (i == (check_outputs.phase-1)) {
						check_outputs.errors[i] = state ? 0x00 : 0x01;
					}
					else {
						check_outputs.errors[i] = state ? 0x03 : 0x00;
					}
				}
			}
		}
	}
	else {
		if (get_delta_tick(check_outputs.start_tick) >= CHECK_OUTPUT_IDLE_DURATION) {
			check_outputs.phase = 0;
		}
	}
}

uint8_t outputs_get_short_errors(void) {
	uint8_t err = 0;
	uint8_t mask = 0x1;
	for (uint8_t i=0; i<8; ++i) {
		if (check_outputs.errors[i]) err |= mask;
		mask <<= 1;
	}
	return err;
}

uint16_t outputs_get_command_code(void) {
	if (outputs_check_command_code(outputs.command_code)) return outputs.command_code;
	return 0x00FF;
}

uint8_t outputs_set_command_code(uint16_t code) {
	if (outputs_check_command_code(code)) {
		outputs.command_code = code;
		return 0;
	}
	outputs.command_code = 0x00FF;
	return 1;
}

void outputs_reset_command(void) {
	for (uint8_t i=0; i<8; ++i) {
		outputs.durations[i] = 0;
	}
	outputs.command_phase = OUTPUTS_PHASE_IDLE;
	outputs_set_command_code(0x00FF);
}

uint8_t outputs_set_preliminary_command(uint16_t code) {
	if (!outputs_check_command_code(code)) return 0x01; // Error: wrong command code;
	if (outputs.command_phase != OUTPUTS_PHASE_IDLE) return 0x02; // Error: another command is expected
	if (outputs_get_short_errors()) return 0x03; // Error: device failure

	outputs_set_command_code(code);
	outputs.command_phase = OUTPUTS_PHASE_PRELIMINARY;
	outputs.wait_start_tick = HAL_GetTick();

	return 0x0;
}

uint8_t outputs_set_executive_command(uint16_t code, uint16_t duration, bool single) {
	uint8_t mask = 0x1;

	if (!outputs_check_command_code(code)) return 0x01; // Error: wrong command code;
	if (single) {
		if (outputs.command_phase != OUTPUTS_PHASE_IDLE) return 0x02; // Error: another command is expected
	}
	else {
		if (code != outputs_get_command_code()) return 0x01; // Error: wrong command code;
		if (outputs.command_phase != OUTPUTS_PHASE_PRELIMINARY) return 0x02; // Error: another command is expected
	}
	if (outputs_get_short_errors()) return 0x03; // Error: device failure

	outputs_set_command_code(code);
	for (uint8_t i=0; i<8; ++i) {
		if (code&mask) outputs_start_command(i, 100*(uint32_t)duration, false);
		mask <<= 1;
	}
	outputs.duration=100*(uint32_t)duration;

	outputs.command_phase = OUTPUTS_PHASE_EXECUTE;
	outputs.wait_start_tick = HAL_GetTick();

	return 0x0;
}

struct outputs_status_t outputs_get_command_status(void) {
	struct outputs_status_t status;
	uint32_t delta;
	status.wait_time=0;


	switch (outputs.command_phase) {
		case OUTPUTS_PHASE_IDLE:
			status.return_code=0x0;
			break;
		case OUTPUTS_PHASE_PRELIMINARY:
			status.return_code=0x1;
			status.wait_time=get_delta_tick(outputs.wait_start_tick)/100;
			break;
		case OUTPUTS_PHASE_EXECUTE:
			status.return_code=0x3;
			delta=get_delta_tick(outputs.wait_start_tick);
			if (delta<outputs.duration) status.wait_time=outputs.duration-delta;
			break;
		default:
			status.return_code=0x4;
		if (outputs_get_short_errors()) status.return_code=0x4;
	}

	status.command_code=outputs_get_command_code();


	return status;
}

uint8_t outputs_get_short_state(void) {
	return outputs_get_short_errors();
}

void outputs_get_extended_state(uint8_t* dst) {
	for (uint8_t i=0; i<8; ++i) {
		*dst++ = check_outputs.errors[i];
	}
}

