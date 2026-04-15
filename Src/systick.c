/*
 * systick.c
 *
 *  Created on: Apr 11, 2026
 *      Author: hillary
 */

#include "systick.h"

#define CTRL_ENABLE (1U << 0)
#define CTRL_CLCKSRC (1U << 2)
#define CTRL_COUNTFLAG (1U << 16)

// Default speed of mcu is 16MHZ
#define ONE_MESC_LOAD 16000

void systick_mesc_delay(uint32_t delay){
	/* Load number of clock cycles per ms */
	SysTick->LOAD = ONE_MESC_LOAD - 1;

	/* Clear systick current value register */
	SysTick->VAL = 0;

	/* Select Internal clock source */
	SysTick->CTRL = CTRL_CLCKSRC;

	/* Enable Systick */
	SysTick->CTRL |= CTRL_ENABLE;

	for (int i = 0; i < delay; i++){
		while((SysTick->CTRL & CTRL_COUNTFLAG) == 0){}
	}

	SysTick->CTRL = 0;
}
