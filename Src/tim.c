/*
 * tim.c
 *
 *  Created on: Apr 11, 2026
 *      Author: hillary
 */


#include "tim.h"

#define TIM2EN (1U << 0)
#define CR1_CEN (1U << 0)

void tim2_1hz_init(void){
	/* Enable clock Access to tim2 */
	RCC->APB1ENR |= TIM2EN;

	/* Set prescaler value */
	TIM2->PSC = 1600 - 1;

	/* Set Auto Reload Value */
	TIM2->ARR = 10000 - 1;

	/* Clear Counter */
	TIM2->CNT = 0;

	/* Enable Timer */
	TIM2->CR1 = CR1_CEN;
}
