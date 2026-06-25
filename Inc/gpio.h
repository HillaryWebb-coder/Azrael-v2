/*
 * gpio.h
 *
 *  Created on: 9 Jun 2026
 *      Author: hilla
 */

#ifndef GPIO_H_
#define GPIO_H_

#include "stm32f4xx.h"

void init_leds();
void led_on(uint8_t led_pin, GPIO_TypeDef * led_gpio);
void led_off(uint8_t led_pin, GPIO_TypeDef * led_gpio);
void disp_cs_high();
void disp_cs_low();
void disp_dc_high();
void disp_dc_low();
void disp_rst_high();
void disp_rst_low();

#endif /* GPIO_H_ */
