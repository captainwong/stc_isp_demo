#include <libemb/emb_time.h>

#ifdef EMB_ENABLE_CUSTOM_TIME

#define SECS_PER_MIN 60L
#define SECS_PER_HOUR 3600L
#define SECS_PER_DAY 86400L

static const int EMB_CODE_MODIFIER month_days[12] = {
    31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

#define days_in_month(x) ((x == 1) ? days_in_feb : month_days[x])

static const int EMB_CODE_MODIFIER days_before_month[12] = {
    0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};

#define is_leap_year(year) ((year % 4 == 0) && ((year % 100 != 0) || (year % 400 == 0)))
#define days_in_year(year) (is_leap_year(year) ? 366 : 365)

static void validate_tm(struct tm* timeptr) {
    int days_in_feb = 28;

    if (timeptr->tm_sec < 0 || timeptr->tm_sec >= 60) {
        timeptr->tm_min += timeptr->tm_sec / 60;
        timeptr->tm_sec %= 60;
        if (timeptr->tm_sec < 0) {
            timeptr->tm_sec += 60;
            timeptr->tm_min--;
        }
    }

    if (timeptr->tm_min < 0 || timeptr->tm_min >= 60) {
        timeptr->tm_hour += timeptr->tm_min / 60;
        timeptr->tm_min %= 60;
        if (timeptr->tm_min < 0) {
            timeptr->tm_min += 60;
            timeptr->tm_hour--;
        }
    }

    if (timeptr->tm_hour < 0 || timeptr->tm_hour >= 24) {
        timeptr->tm_mday += timeptr->tm_hour / 24;
        timeptr->tm_hour %= 24;
        if (timeptr->tm_hour < 0) {
            timeptr->tm_hour += 24;
            timeptr->tm_mday--;
        }
    }

    if (timeptr->tm_mon < 0 || timeptr->tm_mon >= 12) {
        timeptr->tm_year += timeptr->tm_mon / 12;
        timeptr->tm_mon %= 12;
        if (timeptr->tm_mon < 0) {
            timeptr->tm_mon += 12;
            timeptr->tm_year--;
        }
    }

    if (days_in_year(timeptr->tm_year) == 366) {
        days_in_feb = 29;
    }

    if (timeptr->tm_mday <= 0) {
        while (timeptr->tm_mday <= 0) {
            if (--timeptr->tm_mon < 0) {
                timeptr->tm_mon = 11;
                timeptr->tm_year--;
                days_in_feb = is_leap_year(timeptr->tm_year) ? 29 : 28;
            }
            timeptr->tm_mday += days_in_month(timeptr->tm_mon);
        }
    } else {
        while (timeptr->tm_mday > days_in_month(timeptr->tm_mon)) {
            timeptr->tm_mday -= days_in_month(timeptr->tm_mon);
            if (++timeptr->tm_mon >= 12) {
                timeptr->tm_mon = 0;
                timeptr->tm_year++;
                days_in_feb = is_leap_year(timeptr->tm_year) ? 29 : 28;
            }
        }
    }
}

time_t mktime(struct tm* timeptr) {
    time_t t = 0;
    unsigned long days = 0;
    unsigned long year;

    validate_tm(timeptr);

    // compute hours, minutes, seconds
    t += timeptr->tm_sec + timeptr->tm_min * SECS_PER_MIN + timeptr->tm_hour * SECS_PER_HOUR;

    // compute days in year
    days += timeptr->tm_mday;
    days += days_before_month[timeptr->tm_mon];
    if (timeptr->tm_mon > 1 && is_leap_year(timeptr->tm_year)) {
        days++;
    }

    // compute days of the year
    timeptr->tm_yday = days;

    if (timeptr->tm_year > 10000) {
        return (time_t)-1;
    }

    // compute days in other years
    if ((year = timeptr->tm_year) > 70) {
        for (year = 70; year < timeptr->tm_year; year++) {
            days += days_in_year(year);
        }
    } else if (year < 70) {
        for (year = 69; year >= timeptr->tm_year; year--) {
            days -= days_in_year(year);
        }
    }

    // compute total seconds
    t += (time_t)days * SECS_PER_DAY;

    // compute days of the week
    if ((timeptr->tm_wday = (days + 4) % 7) < 0) {
        timeptr->tm_wday += 7;
    }

    return t;
}

#endif  // EMB_ENABLE_CUSTOM_TIME
