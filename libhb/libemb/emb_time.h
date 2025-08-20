#ifndef __EMB_TIME_H__
#define __EMB_TIME_H__

#include "emb_config.h"

#ifdef EMB_ENABLE_CUSTOM_TIME

#ifdef __cplusplus
extern "C" {
#endif

#if !(defined(__C51__) || defined(__SDCC)) || defined(VSCODE)
#pragma pack(1)
#endif

typedef uint32_t time_t;

// user should init sys_time to a correct value
// and increase sys_time by 1 every second, like in a timer interrupt
// otherwise the time will not be correct
extern volatile time_t EMB_DATA_MODIFIER sys_time;

// cautions: the struct tm is not standard, it's just for this lib
// and the value type are not the same as the standard time.h
// we use as less memory as possible
struct tm {
    uint8_t tm_sec;    // 秒
    uint8_t tm_min;    // 分
    uint8_t tm_hour;   // 时
    uint8_t tm_mday;   // 一个月中的日期
    uint8_t tm_mon;    // 月份
    uint16_t tm_year;  // 年份
    uint8_t tm_wday;   // 一周中的第几天
    uint16_t tm_yday;  // 一年中的第几天
    uint8_t tm_isdst;  // 夏令时标识符
};

struct tm* localtime_r(const time_t* timep, struct tm* result);
#define _localtime_r localtime_r
struct tm* localtime(const time_t* timep);
time_t mktime(struct tm* timeptr);
struct tm* gmtime_r(const time_t* timep, struct tm* result);
struct tm* gmtime(const time_t* timep);
time_t time(time_t* timep);

/**
 * @brief convert string time to tm
 *
 * @param stime string time e.g. : Wed Dec 25 05:39:12 2024
 *
 * @return 0 if success
 * @return -1 if length error
 * @return -2 if week error
 * @return -3 if month error
 */
int tm_from_str(const char* stime, struct tm* tm);

/**
 * @brief convert string time to time_t
 *
 * @param stime string time e.g. : Wed Dec 25 05:39:12 2024
 * @return time_t
 */
time_t time_from_str(const char* stime);

#if !(defined(__C51__) || defined(__SDCC)) || defined(VSCODE)
#pragma pack()
#endif

#ifdef __cplusplus
}
#endif

#endif // EMB_ENABLE_CUSTOM_TIME

#endif /* __EMB_TIME_H__ */
