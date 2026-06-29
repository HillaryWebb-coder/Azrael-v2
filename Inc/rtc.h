/*
 * rtc.h
 *
 *  Created on: 11 Jun 2026
 *      Author: hilla
 */

#ifndef RTC_H_
#define RTC_H_

#include <time.h>

void rtc_init(void);
void rtc_get_dateTime(struct tm *t);
void rtc_set_date(uint8_t day, uint8_t month, uint8_t year);
void rtc_set_time(uint8_t time_format, uint8_t hours, uint8_t mins, uint8_t secs);
uint8_t calcWeekday(uint8_t day, uint8_t month, uint16_t year);

#endif /* RTC_H_ */
