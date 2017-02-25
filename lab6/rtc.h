#ifndef RTC_H
#define RTC_H

#include <minix/drivers.h>
#include "utils.h"

#define RTC_ADDR_REG 	0x70
#define RTC_DATA_REG 	0x71

#define RTC_REG_A 		10
#define RTC_UIP			BIT(7)

#define RTC_SECONDS_ADDR	0
#define RTC_MINUTES_ADDR	2
#define RTC_HOURS_ADDR		4

void wait_valid_rtc();

#endif /* RTC_H */
