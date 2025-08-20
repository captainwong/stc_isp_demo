#include <libemb/emb_time.h>

#ifdef EMB_ENABLE_CUSTOM_TIME

#include <string.h>

volatile time_t EMB_DATA_MODIFIER sys_time = 0;

time_t time(time_t* timep) {
    if (timep) {
        *timep = sys_time;
    }
    return sys_time;
}

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
int tm_from_str(const char* stime, struct tm* tm) {
    const char *p = stime, *end = stime;

    while (*end) end++;
    memset(tm, 0, sizeof(*tm));

    if (end - p < 24) {
        return -1;
    }

    if (strncmp(p, "Mon", 3) == 0) {
        tm->tm_wday = 1;
    } else if (strncmp(p, "Tue", 3) == 0) {
        tm->tm_wday = 2;
    } else if (strncmp(p, "Wed", 3) == 0) {
        tm->tm_wday = 3;
    } else if (strncmp(p, "Thu", 3) == 0) {
        tm->tm_wday = 4;
    } else if (strncmp(p, "Fri", 3) == 0) {
        tm->tm_wday = 5;
    } else if (strncmp(p, "Sat", 3) == 0) {
        tm->tm_wday = 6;
    } else if (strncmp(p, "Sun", 3) == 0) {
        tm->tm_wday = 0;
    } else {
        return -2;
    }

    p += 4;
    if (strncmp(p, "Jan", 3) == 0) {
        tm->tm_mon = 0;
    } else if (strncmp(p, "Feb", 3) == 0) {
        tm->tm_mon = 1;
    } else if (strncmp(p, "Mar", 3) == 0) {
        tm->tm_mon = 2;
    } else if (strncmp(p, "Apr", 3) == 0) {
        tm->tm_mon = 3;
    } else if (strncmp(p, "May", 3) == 0) {
        tm->tm_mon = 4;
    } else if (strncmp(p, "Jun", 3) == 0) {
        tm->tm_mon = 5;
    } else if (strncmp(p, "Jul", 3) == 0) {
        tm->tm_mon = 6;
    } else if (strncmp(p, "Aug", 3) == 0) {
        tm->tm_mon = 7;
    } else if (strncmp(p, "Sep", 3) == 0) {
        tm->tm_mon = 8;
    } else if (strncmp(p, "Oct", 3) == 0) {
        tm->tm_mon = 9;
    } else if (strncmp(p, "Nov", 3) == 0) {
        tm->tm_mon = 10;
    } else if (strncmp(p, "Dec", 3) == 0) {
        tm->tm_mon = 11;
    } else {
        return -3;
    }

    p += 4;
    tm->tm_mday = *p - '0';
    tm->tm_mday = tm->tm_mday * 10 + (*(p + 1) - '0');

    p += 3;
    tm->tm_hour = *p - '0';
    tm->tm_hour = tm->tm_hour * 10 + (*(p + 1) - '0');

    p += 3;
    tm->tm_min = *p - '0';
    tm->tm_min = tm->tm_min * 10 + (*(p + 1) - '0');

    p += 3;
    tm->tm_sec = *p - '0';
    tm->tm_sec = tm->tm_sec * 10 + (*(p + 1) - '0');

    p += 3;
    tm->tm_year = *p - '0';
    tm->tm_year = tm->tm_year * 10 + (*(p + 1) - '0');
    tm->tm_year = tm->tm_year * 10 + (*(p + 2) - '0');
    tm->tm_year = tm->tm_year * 10 + (*(p + 3) - '0');
    tm->tm_year -= 1900;

    return 0;
}

/**
 * @brief convert string time to time_t
 *
 * @param stime string time e.g. : Wed Dec 25 05:39:12 2024
 * @return time_t
 */
time_t time_from_str(const char* stime) {
    struct tm tm;
    if (tm_from_str(stime, &tm) == 0) {
        return mktime(&tm);
    }
    return 0;
}

#endif  // EMB_ENABLE_CUSTOM_TIME
