#include <libemb/emb_time.h>

#ifdef EMB_ENABLE_CUSTOM_TIME

#define SECS_PER_MIN 60L
#define MIN_PER_HOUR 60L
#define HOUR_PER_DAY 24L

static const int EMB_CODE_MODIFIER month_days[12] = {
    31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

#define is_leap_year(year) ((year % 4 == 0) && ((year % 100 != 0) || (year % 400 == 0)))

struct tm* gmtime_r(const time_t* timep, struct tm* result) {
    time_t time = *timep;
    int days, years;
    int leap;
    int yday, month;
    int seconds;
    int minutes, hours;

    seconds = time % SECS_PER_MIN;
    time /= SECS_PER_MIN;
    minutes = time % MIN_PER_HOUR;
    time /= MIN_PER_HOUR;
    hours = time % HOUR_PER_DAY;
    time /= HOUR_PER_DAY;
    days = time;
    yday = 0;
    years = 1970;

    while (1) {
        leap = is_leap_year(years);
        if (days < 365 + leap) {
            break;
        }
        days -= 365 + leap;
        years++;
    }

    // calc month and yday
    for (month = 0; month < 12; month++) {
        int month_len = month_days[month];
        if (leap && month == 1) {
            month_len++;
        }
        if (days < month_len) {
            yday += days + 1;
            break;
        }
        days -= month_len;
        yday += month_len;
    }

    result->tm_mday = days + 1;
    result->tm_mon = month;
    result->tm_year = years - 1900;
    result->tm_yday = yday;
    result->tm_sec = seconds;
    result->tm_min = minutes;
    result->tm_hour = hours;
    result->tm_wday = (yday + 4) % 7;
    result->tm_isdst = 0;

    return result;
}

struct tm* gmtime(const time_t* timep) {
    static struct tm t;
    return gmtime_r(timep, &t);
}

#endif  // EMB_ENABLE_CUSTOM_TIME
