/*
 * i2c.c
 *
 *  Created on: 10 Jun 2026
 *      Author: hilla
 */

#include "stm32f4xx.h"
#include "rcc.h"
#include "i2c.h"

/* Assumes APB1 = 50 MHz, standard-mode 100 kHz                         */
#define I2C_PCLK_MHZ     50u
#define I2C_CCR_VALUE    250u   /* PCLK / (2 * 100kHz) = 250            */
#define I2C_TRISE_VALUE  51u    /* (PCLK_MHz * 1000ns) + 1 = 51         */

#define I2C_TIMEOUT      10000u

void i2c1_init(void){
	/** Enable Clocks **/
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
	RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

	/** Configure gpios for i2c (PB8 & PB9) **/
	GPIOB->MODER &= ~(GPIO_MODER_MODE8 | GPIO_MODER_MODE9);
	GPIOB->MODER |= (GPIO_MODER_MODE8_1 | GPIO_MODER_MODE9_1);

	GPIOB->OTYPER |= (GPIO_OTYPER_OT8 | GPIO_OTYPER_OT9);
	GPIOB->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR8 | GPIO_OSPEEDER_OSPEEDR9);
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD8 | GPIO_PUPDR_PUPD9);
	GPIOB->PUPDR |= GPIO_PUPDR_PUPD8_0 | GPIO_PUPDR_PUPD9_0;
	GPIOB->AFR[1] &= ~(GPIO_AFRH_AFRH0 | GPIO_AFRH_AFRH1);
	GPIOB->AFR[1] |= (GPIO_AFRH_AFRH0_2 | GPIO_AFRH_AFRH1_2);

	/** Configure I2C **/
    /* Configure */
	I2C1->CR1 |=  I2C_CR1_SWRST;
	I2C1->CR1 &= ~I2C_CR1_SWRST;

    I2C1->CR2   = I2C_PCLK_MHZ;            /* peripheral clock in MHz  */
    I2C1->CCR   = I2C_CCR_VALUE;           /* standard mode, 100 kHz   */
    I2C1->TRISE = I2C_TRISE_VALUE;

    I2C1->CR1  |= I2C_CR1_PE;              /* enable peripheral        */
}

void i2c1_start(){
	while ((I2C1->SR2 & I2C_SR2_BUSY));
	I2C1->CR1 |= (I2C_CR1_ACK | I2C_CR1_START);
	while(!(I2C1->SR1 & I2C_SR1_SB));
}

void i2c1_write(uint8_t * data, uint32_t size){
//	I2C1->DR = (addr << 1) | 0x00;
//	while (!(I2C1->SR1 & I2C_SR1_ADDR));
//	(void)I2C1->SR2;
	while(size){
		while(!(I2C1->SR1 & I2C_SR1_TXE));
		I2C1->DR = *data;
		data++;
		size--;
		while(!(I2C1->SR1 & I2C_SR1_BTF));
	}

//	while(!(I2C1->SR1 & I2C_SR1_BTF));
//	I2C1->CR1 |= I2C_CR1_STOP;

}

ErrorStatus i2c1_addr(uint8_t addr) {
    uint32_t timeout = I2C_TIMEOUT;
    I2C1->DR = (addr << 1) | 0x00;

    while (timeout--) {
        uint16_t sr1 = I2C1->SR1;
        if (sr1 & I2C_SR1_ADDR) {
            (void)I2C1->SR2;   // clear ADDR
            return SUCCESS;
        }
        if (sr1 & I2C_SR1_AF) {
            I2C1->SR1 &= ~I2C_SR1_AF;
            I2C1->CR1 |= I2C_CR1_STOP;
            return ERROR;
        }
    }

    I2C1->CR1 |= I2C_CR1_STOP;
    return ERROR;
}

void i2c1_read(uint8_t addr, uint8_t * data, uint32_t size){
	addr = (addr << 1) | 0x01;
	if(size == 1){
		I2C1->DR = addr;
		while(!(I2C1->SR1 & I2C_SR1_ADDR));
		I2C1->CR1 &= ~I2C_CR1_ACK;
		(void)I2C1->SR1;
		(void)I2C1->SR2;
		I2C1->CR1 |= I2C_CR1_STOP;

		while(!(I2C1->SR1 & I2C_SR1_RXNE));
		*data = I2C1->DR;
		size--;
	} else {
		I2C1->DR = addr;
		while(!(I2C1->SR1 & I2C_SR1_ADDR));
		(void)I2C1->SR1;
		(void)I2C1->SR2;

		I2C1->CR1 |= I2C_CR1_ACK;
		while(size > 2){
			while(!(I2C1->SR1 & I2C_SR1_RXNE));
			*data++ = I2C1->DR;
			size--;
		}

		while(!(I2C1->SR1 & I2C_SR1_RXNE));
		*data++ = I2C1->DR;
		I2C1->CR1 &= ~I2C_CR1_ACK;
		I2C1->CR1 |= I2C_CR1_STOP;

		size--;
		while(!(I2C1->SR1 & I2C_SR1_RXNE));
		*data = I2C1->DR;

	}
}

void i2c1_stop(void){
		while(!(I2C1->SR1 & I2C_SR1_BTF));
		I2C1->CR1 |= I2C_CR1_STOP;
}
