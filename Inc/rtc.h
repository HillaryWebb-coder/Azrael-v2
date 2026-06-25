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

#endif /* RTC_H_ */
