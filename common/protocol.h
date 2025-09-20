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
 *   len: length of the packet body, from `cmd` to last byte of `data`
 *   cmd: command code
 *     - A0: Connect, bootloader should reply with its version
 *     - A1: Read bytes
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

#include <libemb/emb_config.h>

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

#if !(defined(__C51__) || defined(__SDCC)) || defined(VSCODE)
#pragma pack(1)
#endif

#define PKT_MAX_LEN 255

////////////////////////////// programmer/ota-server packet //////////////////////////////

#define ISP_PKT_HEAD '#'
#define ISP_PKT_END '$'
// A* programmer <--> bootloader
#define ISP_CMD_CONNECT 0xA0
#define ISP_CMD_READ 0xA1
#define ISP_CMD_PROGRAM 0xA2
#define ISP_CMD_ERASE_APP_AREA 0xA3
#define ISP_CMD_REBOOT 0xA4
#define ISP_CMD_ERASE_PAGE 0xA5  // erase 1 page
// B* programmer <--> bootloader/app
#define ISP_CMD_READ_CHIP_INFO 0xB0
#define ISP_CMD_READ_CHIP_VERSION 0xB1
#define ISP_CMD_READ_W25Q_SIZE 0xB2
#define ISP_CMD_ERASE_W25Q_ALL 0xB3     // erase all
#define ISP_CMD_ERASE_W25Q_SECTOR 0xB4  // erase 4KB sector
#define ISP_CMD_READ_W25Q 0xB5
#define ISP_CMD_PROGRAM_W25Q 0xB6
#define ISP_CMD_READ_LDR_VERSION 0xB7  // read bootloader version & build time
#define ISP_CMD_READ_APP_VERSION 0xB8  // read application version & build time
#define ISP_CMD_CALC_CRC32 0xB9        // calculate crc32 of given data
// C* app <--> ota server
#define OTA2APP_CMD_LATEST_APP_INFO 0xC0  // latest application info
#define OTA2APP_CMD_APP_DATA 0xC1         // application data

typedef union {
    uint8_t buf[PKT_MAX_LEN];
    struct {
        uint8_t head;
        uint8_t len;
        uint8_t cmd;
        uint16_t unknown;
        uint16_t addr;
        uint8_t size;
        uint8_t dat[1];  // variable length data
    } pkt;
} isp_packet_t;

#define isp_pkt_end(_pkt) (((isp_packet_t*)_pkt)->buf[2 + ((isp_packet_t*)_pkt)->pkt.len])
#define isp_pkt_sum(_pkt) (((isp_packet_t*)_pkt)->buf[3 + ((isp_packet_t*)_pkt)->pkt.len])
#define isp_pkt_len(_pkt) (2 + ((isp_packet_t*)_pkt)->pkt.len + 2)  // include head,len,end,sum

typedef enum {
    ISP_PARSE_STATE_IDLE = 0,
    ISP_PARSE_STATE_LENGTH,
    ISP_PARSE_STATE_BODY,
    ISP_PARSE_STATE_END,
    ISP_PARSE_STATE_CHECKSUM,
} isp_pkt_parse_state_t;

typedef struct {
    uint8_t state;
    uint8_t len;
    uint8_t sum;
} isp_pkt_parse_context_t;

#define isp_parse_init(ctx) ((ctx).state = ISP_PARSE_STATE_IDLE)

extern bit isp_parse_ok;

#ifdef _MSC_VER  // for programmer
void isp_parse(isp_pkt_parse_context_t* ctx, isp_packet_t* rx, uint8_t b);
#endif

uint8_t isp_pkt_calc_sum(isp_packet_t* pkt);

////////////////////////////// bootloader/app packet //////////////////////////////

#define LDR_PKT_HEAD '@'
#define LDR_PKT_END '$'
#define LDR_STATUS_OK 0
#define LDR_STATUS_UNKNOWN_CMD 1
#define LDR_STATUS_ADDR_OUT_OF_RANGE 2
#define LDR_STATUS_PROGRAM_FAILED 3
#define LDR_STATUS_ERASE_PAGE_FAILED 4
#define LDR_STATUS_CHIP_INFO 0x80
#define LDR_STATUS_CHIP_VERSION 0x81
#define LDR_STATUS_LOG 0x82
#define LDR_STATUS_ROM 0x83  // read ROM response
#define LDR_STATUS_W25Q_SIZE 0x84
#define LDR_STATUS_W25Q_ERASE_ALL_RES 0x85
#define LDR_STATUS_W25Q_ERASE_SECTOR_RES 0x86
#define LDR_STATUS_W25Q_DATA 0x87
#define LDR_STATUS_W25Q_PROGRAM_RES 0x88
#define LDR_STATUS_LDR_VERSION 0x89
#define LDR_STATUS_APP_VERSION 0x8A
#define LDR_STATUS_CALC_CRC32_RES 0x8B
// C* app <--> ota server
#define APP2OTA_CMD_GET_LATEST_APP_INFO 0xC0
#define APP2OTA_CMD_GET_APP_DATA 0xC1

typedef union {
    uint8_t buf[PKT_MAX_LEN];
    struct {
        uint8_t head;
        uint8_t status;  // 0: success, 1: unknown cmd, 2: addr out of range, 3: program failed
        uint8_t size;    // dat length
        uint8_t dat[1];  // variable length data
    } pkt;
} ldr_packet_t;

#define ldr_pkt_end(_pkt) (((ldr_packet_t*)_pkt)->buf[3 + ((ldr_packet_t*)_pkt)->pkt.size])
#define ldr_pkt_sum(_pkt) (((ldr_packet_t*)_pkt)->buf[4 + ((ldr_packet_t*)_pkt)->pkt.size])
#define ldr_pkt_len(_pkt) (5 + ((ldr_packet_t*)_pkt)->pkt.size)  // include head,status,size,end,sum

typedef enum {
    LDR_PARSE_STATE_IDLE = 0,
    LDR_PARSE_STATE_STATUS,
    LDR_PARSE_STATE_SIZE,
    LDR_PARSE_STATE_DATA,
    LDR_PARSE_STATE_END,
    LDR_PARSE_STATE_CHECKSUM
} ldr_parse_state_t;

typedef struct {
    uint8_t state;
    uint8_t len;
    uint8_t sum;
} ldr_pkt_parse_context_t;

extern bit ldr_parse_ok;

#ifdef _MSC_VER  // for programmer
void ldr_parse(ldr_pkt_parse_context_t* ctx, ldr_packet_t* rx, uint8_t b);
#endif

uint8_t ldr_pkt_calc_sum(ldr_packet_t* pkt);

////////////////////////////// ota related structures //////////////////////////////

#define OTA_OK 0
#define OTA_SERVER_ERROR 1
#define OTA_UNKNOWN_VERSION 2
#define OTA_OFFSET_OUT_OF_RANGE 3

// OTA2APP_CMD_LATEST_APP_INFO dat, little-endian
typedef struct {
    uint8_t status;  // 0: success, 1: server error
    app_info_t info;
} latest_app_info_t;

// APP2OTA_CMD_GET_APP_DATA dat, little-endian
typedef struct {
    uint32_t version;
    uint32_t offset;
    uint32_t size;
} get_app_data_req_t;

// OTA2APP_CMD_APP_DATA dat, little-endian
typedef struct {
    uint8_t status;   // 0: success, 2: unknown version, 3: offset out of range
    uint32_t offset;  // offset of the data
    uint32_t size;    // size of the data
    uint32_t crc;     // crc32 of the data
    uint8_t dat[1];   // variable length data
} get_app_data_res_t;

#if !(defined(__C51__) || defined(__SDCC)) || defined(VSCODE)
#pragma pack()
#endif

#ifdef __cplusplus
}
#endif

#endif /* __PROTOCOL_H__ */
