/*
 * gpio.c
 *
 *  Created on: Apr 10, 2026
 *      Author: hillary
 */
#include "gpio.h"

#define GPIOCEN (1U << 2)
#define LED 13
#define LED_BSR (1U << 13)
#define LED_BRR (1U << 29)

#define GPIOAEN (1U << 0)
#define BTN 0
#define BTN_IDR (1U << BTN)


void led_init (void){
	RCC->AHB1ENR |= GPIOCEN;
	GPIOC->MODER |= (1U << (LED * 2));
	GPIOC->MODER |= ~(1U << ((LED * 2) + 1));

	GPIOA->MODER |= (1U << 4);
	GPIOA->MODER &= ~(1U << 5);
}

void button_init (void){
	RCC->AHB1ENR |= GPIOAEN;
	GPIOA->MODER &= ~(3U << (BTN * 2));

	GPIOA->PUPDR &= ~(3U << (BTN * 2));
	GPIOA->PUPDR |= (1U << (BTN * 2));
}

void led_on (void){
	GPIOC->BSRR |= LED_BSR;
}

void led_off (void) {
	GPIOC->BSRR |= LED_BRR;
}

void led_toggle (void){
	GPIOC->ODR ^= (1U << LED);
}

bool get_btn_state(void){
	if(GPIOA->IDR & BTN_IDR){
		return false;
	}

	return true;
}


