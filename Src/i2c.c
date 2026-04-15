/*
 * i2c.c
 *
 *  Created on: Apr 11, 2026
 *      Author: hillary
 */


#include "i2c.h"

#define GPIOBEN (1U << 1)
#define I2C1EN (1U << 21)

#define I2C_100KHZ 80
#define SD_MODE_MAX_RISE_TIME 17
#define CR1_PE (1U << 0)

#define SR2_BUSY (1U << 1)
#define CR1_START (1U << 8)
#define SR1_SB (1U << 0)
#define SR1_ADDR (1U << 1)
#define SR1_TXE (1U << 7)
#define CR1_ACK (1U << 10)
#define CR1_STOP (1U << 9)
#define SR1_RXNE (1U << 6)
#define SR1_BTF (1U << 2)


void i2c1_init(void){
	/* Enable clock access to GPIOB */
	RCC->AHB1ENR |= GPIOBEN;

	/*Set PB8 and PB9 to alternate funtion mode */
	GPIOB->MODER |= (2U << 16);
	GPIOB->MODER |= (2U << 18);

	/* Set PB8 and PB9 output type to open drain */
	GPIOB->OTYPER |= (1U << 8);
	GPIOB->OTYPER |= (1U << 9);

	/* Enable Pullups for PB8 and PB9 */
	GPIOB->PUPDR &= ~(3U << 16);
	GPIOB->PUPDR |= (1U << 16);

	GPIOB->PUPDR &= ~(3U << 18);
	GPIOB->PUPDR |= (1U << 18);

	/* Set PB8 and PB9 alternate functions to I2C */
	GPIOB->AFR[1] &= ~(15U << 0);
	GPIOB->AFR[1] |= (4U << 0);

	GPIOB->AFR[1] &= ~(15U << 4);
	GPIOB->AFR[1] |= (4U << 4);

	/* Enable clock access to I2C1 */
	RCC->APB1ENR |= I2C1EN;

	/* Enter Reset Mode */
	I2C1->CR1 |= (1U << 15);

	/* Exit reset mode */
	I2C1->CR1 &= ~(1U << 15);

	/* Set peripheral clock frequency */
	I2C1->CR2 = (1U << 4);

	/* set I2C to standard mode, 100Khz clock */
	I2C1->CCR = I2C_100KHZ;

	/* set rise time */
	I2C1->TRISE = SD_MODE_MAX_RISE_TIME;

	/* Enable I2C1 Module */
	I2C1->CR1 |= CR1_PE;
}


void i2c1_byte_read(char saddr, char maddr, char *data){
	volatile int tmp;

	/* Wait until bus not busy */
	while(I2C1->SR2 & (SR2_BUSY));

	/* Generate start */
	I2C1->CR1 |= CR1_START;

	/* Wait until start flag is set */
	while(!(I2C1->SR1 & (SR1_SB)));

	/* Transmit slave adress + Write */
	I2C1->DR = saddr << 1;

	/* Wait until tje addr flag is set */
	while (!(I2C1->SR1 & (SR1_ADDR)));

	/* clear addr flag */
	tmp = I2C1->SR2;

	/* Send memory address */
	I2C1->DR = maddr;

	/* Wait until transmitter is empty */
	while(!(I2C1->SR1 & SR1_TXE));

	/* Generate restart */
	I2C1->CR1 |= CR1_START;

	/* Wait until the start flag is set */
	while(!(I2C1->SR1 & SR1_SB));

	/* Transmit slave addresss + Read */
	I2C1->DR = saddr << 1 | 1U;

	/* wait until the addr flag is set */
	while(!(I2C1->SR1 & (SR1_ADDR)));

	/* Disable acknowlege */
	I2C1->CR1 &= ~CR1_ACK;

	/* Clear addr flag */
	tmp = I2C1->SR2;

	/* Generate stop after data recieved */
	I2C1->CR1 |= CR1_STOP;

	/* Wait until RXNE flag is set */
	while (!(I2C1->SR1 & SR1_RXNE));

	/* Read data from DR */
	*data++ = I2C1->DR;
}



















