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

#include <libemb/emb_config.h>

#ifdef __cplusplus
extern "C" {
#endif

#if !(defined(__C51__) || defined(__SDCC)) || defined(VSCODE)
#pragma pack(1)
#endif

#define PKT_MAX_LEN 255

////////////////////////////// programmer packet //////////////////////////////

#define ISP_PKT_HEAD '#'
#define ISP_PKT_END '$'
#define ISP_CMD_CONNECT 0xA0
#define ISP_CMD_READ 0xA1
#define ISP_CMD_PROGRAM 0xA2
#define ISP_CMD_ERASE 0xA3
#define ISP_CMD_REBOOT 0xA4
#define ISP_CMD_READ_CHIP_INFO 0xB0

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

////////////////////////////// bootloader packet //////////////////////////////

#define LDR_PKT_HEAD '@'
#define LDR_PKT_END '$'
#define LDR_STATUS_OK 0
#define LDR_STATUS_UNKNOWN_CMD 1
#define LDR_STATUS_ADDR_OUT_OF_RANGE 2
#define LDR_STATUS_PROGRAM_FAILED 3
#define LDR_STATUS_CHIP_INFO 0x80

typedef union {
    uint8_t buf[PKT_MAX_LEN];
    struct {
        uint8_t status;  // 0: success, 1: unknown cmd, 2: addr out of range, 3: program failed
        uint8_t size;
        uint8_t dat[1];  // variable length data
    } pkt;
} ldr_packet_t;

typedef enum {
    LDR_PARSE_STATE_IDLE = 0,
    LDR_PARSE_STATE_STATUS,
    LDR_PARSE_STATE_SIZE,
    LDR_PARSE_STATE_DATA,
    LDR_PARSE_STATE_CHECKSUM
} ldr_parse_state_t;

typedef struct {
    uint8_t state;
    uint8_t len;
    uint8_t sum;
} ldr_pkt_parse_context_t;

#if !(defined(__C51__) || defined(__SDCC)) || defined(VSCODE)
#pragma pack()
#endif

#ifdef __cplusplus
}
#endif

#endif /* __PROTOCOL_H__ */
