/*
 * dio_dma.c
 *
 *  Created on: Feb 7, 2024
 *      Author: lepikhov
 */
#include "main.h"
#include "tim.h"
#include "dio_dma.h"

struct dio_dma_struct_type dio_dma_struct[] = {
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
}
};

void set_dio_dir_in(GPIO_TypeDef * gpio_base, uint8_t bit) {
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

void set_dio_dir_out(GPIO_TypeDef * gpio_base, uint8_t bit) {
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

void dio_dma_init()
{
//	HAL_DMA_Start_IT(htim1.hdma[TIM_DMA_ID_UPDATE], (uint32_t)dio_dma.buff, (uint32_t)dio_dma.bsrr, DIO_DMA_BUFFER_SIZE);
//	__HAL_TIM_ENABLE_DMA(&htim1, TIM_DMA_UPDATE);
//	__HAL_TIM_ENABLE(&htim1);
}

void dio_dma_func()
{
	for (int i=0; i<sizeof(dio_dma_struct)/sizeof(dio_dma_struct[0]); ++i) {

			set_dio_dir_out((GPIO_TypeDef *)dio_dma_struct[i].gpio_base, dio_dma_struct[i].bit);
			HAL_DMA_Start_IT(htim1.hdma[TIM_DMA_ID_UPDATE],
					(uint32_t)dio_dma_struct[i].buff,
					(uint32_t)dio_dma_struct[i].bsrr,
					DIO_DMA_BUFFER_SIZE);

			__HAL_TIM_ENABLE_DMA(&htim1, TIM_DMA_UPDATE);
			__HAL_TIM_ENABLE(&htim1);

			//HAL_TIM_Base_Start_IT(&htim1); // start timer1
			HAL_Delay(500);

			//HAL_TIM_Base_Stop_IT(&htim1); // stop timer

			__HAL_TIM_DISABLE(&htim1);
			__HAL_TIM_DISABLE_DMA(&htim1, TIM_DMA_UPDATE);
			HAL_DMA_Abort_IT(htim1.hdma[TIM_DMA_ID_UPDATE]);


			((GPIO_TypeDef *)dio_dma_struct[i].gpio_base)->BSRR = dio_dma_struct[i].buff[1];
			//HAL_Delay(5000);

			set_dio_dir_in((GPIO_TypeDef *)dio_dma_struct[i].gpio_base, dio_dma_struct[i].bit);


	}
}


