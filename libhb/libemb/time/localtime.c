#include <libemb/emb_time.h>

#ifdef EMB_ENABLE_CUSTOM_TIME

struct tm* localtime_r(const time_t* timep, struct tm* result) {
    return gmtime_r(timep, result);
}

struct tm* localtime(const time_t* timep) {
    return gmtime(timep);
}

#endif // EMB_ENABLE_CUSTOM_TIME
