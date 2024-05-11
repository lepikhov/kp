/*
 * dio_dma.h
 *
 *  Created on: Feb 7, 2024
 *      Author: lepikhov
 */

#ifndef SRC_DIO_DMA_H_
#define SRC_DIO_DMA_H_

#define DIO_DMA_BUFFER_SIZE 5

struct dio_dma_struct_type{
	uint32_t buff[DIO_DMA_BUFFER_SIZE];
	uint32_t bsrr;
	uint32_t gpio_base;
	uint8_t  bit;
};

void dio_dma_init();
void dio_dma_func();

void set_dio_dir_in(GPIO_TypeDef * gpio_base, uint8_t bit);
void set_dio_dir_out(GPIO_TypeDef * gpio_base, uint8_t bit);

#endif /* SRC_DIO_DMA_H_ */
