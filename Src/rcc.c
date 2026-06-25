/*
 * rcc.c
 *
 *  Created on: 9 Jun 2026
 *      Author: hilla
 */

#include "stm32f4xx.h"
#include "stm32f411xe.h"
#include "rcc.h"

#define CTRL_CLKSRC (1UL << 2)
#define CTRL_ENABLE (1UL << 0)
#define CTRL_TICKINT (1UL << 1)
#define CTRL_CNTFLAG (1UL << 16)
#define ONE_MESC_LOAD 1000

#define PLL_M 25UL
#define PLL_N 192UL
#define PLL_P 0 // PLLP = 2
#define PLL_Q 4UL

void initialize_clocks(){
	RCC->CR |= RCC_CR_HSEON;
	while(!(RCC->CR & RCC_CR_HSERDY));
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;

	/** Configure power and flash wait states **/
	PWR->CR |= PWR_CR_VOS;
	FLASH->ACR |= (FLASH_ACR_DCEN | FLASH_ACR_ICEN | FLASH_ACR_PRFTEN | FLASH_ACR_LATENCY_3WS);

	/** Configure rcc prescalers **/
	RCC->CFGR &= ~(RCC_CFGR_PPRE2 | RCC_CFGR_HPRE);
	RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;

	/** Configure pll **/
	RCC->PLLCFGR |= ~(RCC_PLLCFGR_PLLM | RCC_PLLCFGR_PLLN | RCC_PLLCFGR_PLLQ | RCC_PLLCFGR_PLLP);
	RCC->PLLCFGR |= ((PLL_M << 0) | (PLL_N << 6) | (PLL_P << 16) | (PLL_Q << 24) | RCC_PLLCFGR_PLLSRC_HSE);
	RCC->CR |= RCC_CR_PLLON;
	while(!(RCC->CR & RCC_CR_PLLRDY));

	/** Select system clock source **/
	RCC->CFGR |= RCC_CFGR_SW_PLL;
	while(!(RCC->CFGR & RCC_CFGR_SWS_PLL));
	SystemCoreClockUpdate();
}
