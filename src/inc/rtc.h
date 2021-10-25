#ifndef _RTC_H_
#define _RTC_H_


#include <stdint.h>

/* ts: seconds since 1970-01-01 00:00:00 UTC */
extern uint32_t time_ts(void);
extern int time_sync(uint32_t ts);
extern int is_time_sync(void);

extern int rtc_init(void);
extern int rtc_deinit(void);


#endif /* _RTC_H_ */
