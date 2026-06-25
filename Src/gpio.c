/*
 * gpio.c
 *
 *  Created on: 9 Jun 2026
 *      Author: hilla
 */
#include <stdint.h>

#include "stm32f4xx.h"
#include "stm32f411xe.h"

#include "main.h"
#include "gpio.h"

void init_leds(){
	RCC->AHB1ENR |= ((1UL << 1) | (1UL << 2));

	LED_GREEN_GPIO->MODER &= ~(3UL << (LED_GREEN_PIN * 2));
	LED_GREEN_GPIO->MODER |= (1UL << (LED_GREEN_PIN * 2));

	LED_STATUS_GPIO->MODER &= ~(3UL << (LED_STATUS_PIN * 2));
	LED_STATUS_GPIO->MODER |= (1UL << (LED_STATUS_PIN * 2));

	DISP_CS_GPIO->MODER &= ~(3UL << (DISP_CS_PIN * 2));
	DISP_CS_GPIO->MODER |= (1UL << (DISP_CS_PIN * 2));

	DISP_DC_GPIO->MODER &= ~(3UL << (DISP_DC_PIN * 2));
	DISP_DC_GPIO->MODER |= (1UL << (DISP_DC_PIN * 2));

	DISP_RST_GPIO->MODER &= ~(3UL << (DISP_RST_PIN * 2));
	DISP_RST_GPIO->MODER |= (1UL << (DISP_RST_PIN * 2));
}

void led_on(uint8_t led_pin, GPIO_TypeDef * led_gpio){
	led_gpio->BSRR |= (1UL << led_pin);
}

void led_off(uint8_t led_pin, GPIO_TypeDef * led_gpio){
	led_gpio->BSRR |= (1UL << (led_pin + 16));
}

void disp_cs_high(){
	DISP_CS_GPIO->BSRR |= (1UL << DISP_CS_PIN);
}

void disp_cs_low(){
	DISP_CS_GPIO->BSRR |= (1UL << (DISP_CS_PIN + 16));
}

void disp_dc_high(){
	DISP_DC_GPIO->BSRR |= (1UL << DISP_DC_PIN);
}

void disp_dc_low(){
	DISP_DC_GPIO->BSRR |= (1UL << (DISP_DC_PIN + 16));
}

void disp_rst_high(){
	DISP_RST_GPIO->BSRR |= (1UL << DISP_RST_PIN);
}

void disp_rst_low(){
	DISP_RST_GPIO->BSRR |= (1UL << (DISP_RST_PIN + 16));
}
