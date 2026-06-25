/*
 * tim.c
 *
 *  Created on: 12 Jun 2026
 *      Author: hilla
 */

#include "stm32f4xx.h"
#include "tim.h"

static volatile uint32_t SysTickCounter = 0;

void tim2_1hz_init(void) {
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    TIM2->PSC = 96000 - 1;        // see below
    TIM2->ARR = 1000 - 1;
    TIM2->CNT = 0;

    TIM2->DIER |= TIM_DIER_UIE;    // enable update interrupt
    TIM2->CR1  |= TIM_CR1_CEN;     // start timer
    while(!(TIM2->SR & TIM_SR_UIF));

    NVIC_SetPriority(TIM2_IRQn, 6);
    NVIC_EnableIRQ(TIM2_IRQn);
}

void TIM2_IRQHandler(void) {
    if (TIM2->SR & TIM_SR_UIF) {
        TIM2->SR &= ~TIM_SR_UIF;   // clear update interrupt flag
        SysTickCounter++;
    }
}

void delay_ms(uint32_t ms){
	TIM2->CNT = 0;
	while(TIM2->CNT < ms);
}
