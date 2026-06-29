/*
 * main.h
 *
 *  Created on: 9 Jun 2026
 *      Author: hilla
 */

#ifndef MAIN_H_
#define MAIN_H_

#include "stm32f4xx.h"

#define LED_GREEN_PIN 7
#define LED_GREEN_GPIO GPIOB

#define LED_STATUS_PIN 13
#define LED_STATUS_GPIO GPIOC

#define LED_BLUE_PIN 13
#define LED_BLUE_GPIO GPIOB

#define DISP_CS_PIN 4
#define DISP_CS_GPIO GPIOA

#define DISP_DC_PIN 3
#define DISP_DC_GPIO GPIOA

#define DISP_RST_PIN 6
#define DISP_RST_GPIO GPIOA

#endif /* MAIN_H_ */
