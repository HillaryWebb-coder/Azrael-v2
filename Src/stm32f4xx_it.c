/*
 * stm32f4xx_it.c
 *
 *  Created on: 12 Jun 2026
 *      Author: hilla
 */
#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "task.h"

#include "display.h"

void DMA2_Stream2_IRQHandler(void){
	BaseType_t xHigherPriorityTaskAwoken = pdFALSE;

	if(DMA2->LISR & DMA_LISR_TCIF2){
		DMA2->LIFCR |= DMA_LIFCR_CTCIF2;

		SPI1->CR2 & ~SPI_CR2_TXDMAEN;
		DMA2_Stream2->CR &= ~DMA_SxCR_EN;

		while(SPI1->SR & SPI_SR_BSY);

		if(xDisplayTaskHandle != NULL){
			vTaskNotifyGiveFromISR(xDisplayTaskHandle, &xHigherPriorityTaskAwoken);
			xDisplayTaskHandle = NULL;
		}
	}
	portYIELD_FROM_ISR(xHigherPriorityTaskAwoken);
}
