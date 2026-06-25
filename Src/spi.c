/*
 * spi.c
 *
 *  Created on: 9 Jun 2026
 *      Author: hilla
 */


#include "stm32f4xx.h"
#include "stm32f411xe.h"

#include "spi.h"

void spi1_init(void){

	/** Enable Clocks **/
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

	/** Setup GPIO **/
	GPIOA->MODER &= ~(GPIO_MODER_MODE5 | GPIO_MODER_MODE6 | GPIO_MODER_MODE7);
	GPIOA->MODER |= (GPIO_MODER_MODE5_1 | GPIO_MODER_MODE6_1 | GPIO_MODER_MODE7_1);
	GPIOA->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR5 | GPIO_OSPEEDER_OSPEEDR6 | GPIO_OSPEEDER_OSPEEDR7);
	GPIOA->PUPDR |= GPIO_PUPDR_PUPD6_0;

	GPIOA->AFR[0] &= ~(GPIO_AFRL_AFRL5 | GPIO_AFRL_AFRL6 | GPIO_AFRL_AFRL7);
	GPIOA->AFR[0] |= ((5UL << 20) | (5UL << 24) | (5UL << 28));

	/** Configure SPI1 **/

	SPI1->CR1 &= ~(SPI_CR1_BR);
	SPI1->CR1 |= (1UL << SPI_CR1_BR_Pos);
	SPI1->CR1 &= ~(SPI_CR1_CPHA | SPI_CR1_CPOL | SPI_CR1_RXONLY | SPI_CR1_LSBFIRST | SPI_CR1_DFF);
	SPI1->CR1 |= (SPI_CR1_MSTR | SPI_CR1_SSM | SPI_CR1_SSI);
	SPI1->CR1 |= SPI_CR1_SPE;
}

void spi1_transmit(uint8_t * data, uint32_t size){

	while(size){
		while(!(SPI1->SR & SPI_SR_TXE));
		SPI1->DR = *data;
		data++;
		size--;
	}

	while(!(SPI1->SR & SPI_SR_TXE));
	while(SPI1->SR & SPI_SR_BSY);

	(void)SPI1->DR;
	(void)SPI1->SR;
}

void spi1_receive(uint8_t * data, uint32_t size){
	while(size){
		SPI1->DR = 0;
		while(!(SPI1->SR & SPI_SR_RXNE));
		*data++ = SPI1->DR;
		size--;
	}
}

void spi_dma_init(void){
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;

	DMA2_Stream2->CR = 0;
	while(DMA2_Stream2->CR & DMA_SxCR_EN);

	DMA2_Stream2->CR |= ((2UL << DMA_SxCR_CHSEL_Pos) | DMA_SxCR_MINC | (1UL << DMA_SxCR_DIR_Pos) | DMA_SxCR_TCIE);
	DMA2_Stream2->PAR = (uint32_t)(&(SPI1->DR));

	NVIC_SetPriority(DMA2_Stream2_IRQn, 6);
	NVIC_EnableIRQ(DMA2_Stream2_IRQn);

}

void spi_dma_transmit(uint8_t * buff, uint16_t size){
	DMA2_Stream2->M0AR = (uint32_t)(buff);
	DMA2_Stream2->NDTR = size;

	DMA2->LIFCR |= DMA_LIFCR_CTCIF2;

	DMA2_Stream2->CR |= DMA_SxCR_EN;
	SPI1->CR2 |= SPI_CR2_TXDMAEN;
}
