/**
 * @file protocol.h
 * @brief STC official isp demo protocol reverse engined
 * @author captainwong 1281261856#qq.com
 * @date 2025-8-21
 * 
 * ## programmer packet format
 *   | head | len | cmd | u16 | addr | size | data... | end | sum |
 *   | ---- | --- | --- | --- | ---- | ---- | ------- | --- | --- |
 *   |  '#' |  1  |  1  |  2  |  2   |  1   |   n     | '$' |  1  |
 *   
 *   head: fixed `#`
 *   len: length of the packet, from `cmd` to data[n-1]
 *   cmd: command code
 *     - A0: Connect, bootloader should reply with its version
 *     - A1: Read byte
 *     - A2: Program bytes
 *     - A3: Erase ALL eeprom
 *     - A4: Reboot
 *   u16: unknown yet
 *   addr: address to read/program/erase, BIG-ENDIAN
 *   size: size of the `data` field, BIG-ENDIAN
 *   data: variable length data
 *   end: fixed `$`
 *   sum: checksum, 1 byte, calculated as the sum of all bytes from `head` to `end` then negated, 
 *        also if you sum from `head` to `sum`, you got `0`
 * 
 * ## bootloader packet format
 *   | head | status | size | data... | end | sum |
 *   | ---- | ------ | ---- | ------- | --- | --- |
 *   |  '@' |  1     |  1   |   n     | '$' |  1  |
 * 
 *   head: fixed `@`
 *   status: 
 *     - 0: success
 *     - 1: unkown `cmd`
 *     - 2: `addr` out of range
 *     - 3: program failed
 *   size: length of `data` field
 *   end: fixed `$`
 *   sum: same as programmer packet
 */

#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#if defined(__C51__) || defined(VSCODE)
#include "sys/sys.h"
#else
#include <stdint.h>
#endif

#if !(defined(__C51__) || defined(__SDCC)) || defined(VSCODE)
#pragma pack(1)
#endif

////////////////////////////// programmer packet //////////////////////////////

#define ISP_PKT_HEAD '#'
#define ISP_PKT_END '$'

typedef struct {
    uint8_t len;
    uint8_t cmd;
    uint16_t unknown;
    uint16_t addr;
    uint8_t size;
    uint8_t dat[1]; // variable length data
} isp_packet_t;

////////////////////////////// bootloader packet //////////////////////////////

#define LDR_PKT_HEAD '@'
#define LDR_PKT_END '$'

typedef struct {
    uint8_t status; // 0: success, 1: unknown cmd, 2: addr out of range, 3: program failed
    uint8_t size;
    uint8_t dat[1]; // variable length data
} ldr_packet_t;

#if !(defined(__C51__) || defined(__SDCC)) || defined(VSCODE)
#pragma pack()
#endif

#endif /* __PROTOCOL_H__ */
