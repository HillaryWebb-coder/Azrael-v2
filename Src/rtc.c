/*
 * rtc.c
 *
 *  Created on: 11 Jun 2026
 *      Author: hilla
 */
#include <time.h>
#include <string.h>

#include "stm32f4xx.h"
#include "rtc.h"

#define RTC_WPR_KEY_1 0xCA
#define RTC_WPR_KEY_2 0x53
#define ASYNC_PRESCALER 127UL
#define SYNC_PRESCALER 255UL

static void rtc_set_asynch_prescaler(uint32_t AsyncPrescaler){
	RTC->PRER &= ~RTC_PRER_PREDIV_A;
	RTC->PRER |= (AsyncPrescaler << RTC_PRER_PREDIV_A_Pos);
}

static void rtc_set_synch_prescaler(uint32_t SynchPrescaler){
	RTC->PRER &= ~RTC_PRER_PREDIV_S;
	RTC->PRER |= (SynchPrescaler << RTC_PRER_PREDIV_S_Pos);
}

void _rtc_enable_init_mode(void){
	RTC->ISR |= RTC_ISR_INIT;
}

void _rtc_disable_init_mode(void) {
	RTC->ISR &= ~RTC_ISR_INIT;
}

uint8_t _rtc_isActiveFlag_init(void){
	return RTC->ISR & RTC_ISR_INITF ? 1 : 0;
}

uint8_t _rtc_isActiveFlag_rs(void){
	return RTC->ISR & RTC_ISR_RSF ? 1 : 0;
}

static uint8_t rtc_init_seq(void){

	/** Start Init Mode **/
	_rtc_enable_init_mode();

	while(!_rtc_isActiveFlag_init());

	return 1;
}

static uint8_t wait_for_synchro(void){

	/** clear RSF **/
	RTC->ISR &= ~RTC_ISR_RSF;

	while(!_rtc_isActiveFlag_rs());

	return 1;
}

static uint8_t exit_init_seq(void){

	/** Disable Init Mode **/
	_rtc_disable_init_mode();

	return(wait_for_synchro());
}

static uint8_t decimal_to_bcd(uint8_t val){
	return ((val / 10) << 4) | (val % 10);
}

static uint8_t bcd_to_decimal(uint8_t val){
	return ((val >> 4) * 10) + (val & 0x0F);
}

static void rtc_date_config(uint8_t weekDay, uint8_t day, uint8_t month, uint8_t year){
	RTC->DR = ((weekDay << RTC_DR_WDU_Pos) | (decimal_to_bcd(day) << RTC_DR_DU_Pos) | (decimal_to_bcd(month) << RTC_DR_MU_Pos) | (decimal_to_bcd(year) << RTC_DR_YU_Pos));
}

static void rtc_time_config(uint8_t time_format, uint8_t hours, uint8_t mins, uint8_t secs){
	RTC->TR = ((time_format << RTC_TR_PM_Pos) | (decimal_to_bcd(hours) << RTC_TR_HU_Pos) | (decimal_to_bcd(mins) << RTC_TR_MNU_Pos) | (decimal_to_bcd(secs) << RTC_TR_SU_Pos));
}

void rtc_init(void){
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	PWR->CR |= PWR_CR_DBP;

	if(!(RTC->ISR & RTC_ISR_INITS)){
		RCC->BDCR |= RCC_BDCR_BDRST;
		RCC->BDCR &= ~RCC_BDCR_BDRST;

		RCC->BDCR |= RCC_BDCR_LSEON;
		while(!(RCC->BDCR & RCC_BDCR_LSERDY));

		// set rtc clock source to LSE & enable rtc
		RCC->BDCR &= ~RCC_BDCR_RTCSEL_Msk;
		RCC->BDCR |= RCC_BDCR_RTCSEL_0;
		RCC->BDCR |= RCC_BDCR_RTCEN;

		RTC->WPR = RTC_WPR_KEY_1;
		RTC->WPR = RTC_WPR_KEY_2;

		rtc_init_seq();


		RTC->CR |= RTC_CR_FMT;
		rtc_set_asynch_prescaler(ASYNC_PRESCALER);
		rtc_set_synch_prescaler(SYNC_PRESCALER);

		exit_init_seq();
		RTC->WPR = RTC_WPR_KEY;
	}
}

void rtc_get_dateTime(struct tm *t){
	uint32_t tr = RTC->TR;
	uint32_t dr = RTC->DR;

	t->tm_sec = bcd_to_decimal((tr >> RTC_TR_SU_Pos) & 0x7F);
	t->tm_min = bcd_to_decimal((tr >> RTC_TR_MNU_Pos) & 0x7F);
	t->tm_hour = bcd_to_decimal((tr >> RTC_TR_HU_Pos) & 0x3F);
	t->tm_mday = bcd_to_decimal((dr >> RTC_DR_DU_Pos) & 0x3F);
	t->tm_mon = bcd_to_decimal((dr >> RTC_DR_MU_Pos) & 0x1F) - 1;
	t->tm_year = bcd_to_decimal((dr >> RTC_DR_YU_Pos) & 0xFF) + 100;

	uint8_t rtc_wday = (dr >> RTC_DR_WDU_Pos) & 0x07;
	t->tm_wday = (rtc_wday == 7) ? 0 : rtc_wday;

	t->tm_yday = 0;
	t->tm_isdst = 0;
}

uint8_t calcWeekday(uint8_t day, uint8_t month, uint16_t year){
    // Tomohiko Sakamoto's algorithm, returns 0=Sun ... 6=Sat
    // shift to 1=Mon ... 7=Sun for STM32
    static int t[] = {0,3,2,5,0,3,5,1,4,6,2,4};
    if(month < 3) year--;
    int w = (year + year/4 - year/100 + year/400 + t[month-1] + day) % 7;
    // convert: 0=Sun→7, 1=Mon→1 ... 6=Sat→6
    return w == 0 ? 7 : w;
}

void rtc_set_date(uint8_t day, uint8_t month, uint8_t year){
	RTC->WPR = RTC_WPR_KEY_1;
	RTC->WPR = RTC_WPR_KEY_2;

	rtc_init_seq();
	rtc_date_config(calcWeekday(day, month, year), day, month, year);
	exit_init_seq();
	RTC->WPR = RTC_WPR_KEY;
}

void rtc_set_time(uint8_t time_format, uint8_t hours, uint8_t mins, uint8_t secs){
	RTC->WPR = RTC_WPR_KEY_1;
	RTC->WPR = RTC_WPR_KEY_2;

	rtc_init_seq();
	rtc_time_config(time_format, hours, mins, secs);
	exit_init_seq();
	RTC->WPR = RTC_WPR_KEY;
}

