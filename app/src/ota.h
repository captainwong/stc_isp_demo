#ifndef __OTA_H__
#define __OTA_H__

#include <sys/sys.h>

#include "common.h"
#include "protocol.h"

void ota_init(void);
void ota_1s_event(void);
void ota_on_latest_app_info(const latest_app_info_t* info);
void ota_run(void);

#endif /* __OTA_H__ */
