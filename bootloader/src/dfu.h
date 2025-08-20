#ifndef __DFU_H__
#define __DFU_H__

#include "sys/sys.h"

// #define DFU_FORCEPIN            P33

#define DFU_CMD_CONNECT         0xA0
#define DFU_CMD_READ            0xA1
#define DFU_CMD_PROGRAM         0xA2
#define DFU_CMD_ERASE           0xA3
#define DFU_CMD_REBOOT          0xA4

#define STATUS_OK               0x00
#define STATUS_ERRORCMD         0x01
#define STATUS_OUTOFRANGE       0x02
#define STATUS_PROGRAMERR       0x03
#define STATUS_ERRORWRAP        0xFF

void dfu_check();
void dfu_events();

extern uint32_t xdata dfuflag;
extern char *USER_STCISPCMD;

#endif

