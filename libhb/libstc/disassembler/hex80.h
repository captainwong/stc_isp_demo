/*
https://developer.arm.com/documentation/ka003292/latest

The Intel HEX file is an ASCII text file with lines of text that follow the Intel HEX file format.
Each line in an Intel HEX file contains one HEX record.
These records are made up of hexadecimal numbers that represent machine language code and/or constant data.
Intel HEX files are often used to transfer the program and data that would be stored in a ROM or EPROM.
Most EPROM programmers or emulators can use Intel HEX files.

Record Format
An Intel HEX file is composed of any number of HEX records.
Each record is made up of five fields that are arranged in the following format:

:llaaaatt[dd...]cc
Each group of letters corresponds to a different field, and each letter represents a single hexadecimal digit.
Each field is composed of at least two hexadecimal digits-which make up a byte-as described below:

- : is the colon that starts every Intel HEX record.
- ll is the record-length field that represents the number of data bytes (dd) in the record.
- aaaa is the address field that represents the starting address for subsequent data in the record.
- tt is the field that represents the HEX record type, which may be one of the following:
    - 00 - data record
    - 01 - end-of-file record
    - 02 - extended segment address record
    - 04 - extended linear address record
    - 05 - start linear address record (MDK-ARM only)
- dd is a data field that represents one byte of data. A record may have multiple data bytes.
    The number of data bytes in the record must match the number specified by the ll field.
- cc is the checksum field that represents the checksum of the record.
    The checksum is calculated by summing the values of all hexadecimal digit pairs in the record modulo 256 and taking the two's complement.
*/

#ifndef __INTEL_HEX80_H__
#define __INTEL_HEX80_H__

#include <stdint.h>

#include <string>
#include <vector>

#include "hex.h"

#pragma pack(1)
typedef union {
    uint8_t raw[260];
    struct {
        uint8_t colon;
        uint8_t len;  // dat len
        uint16_t addr;
        uint8_t type;
        uint8_t dat[1];
    } st;
} hex80_record_t;
#pragma pack()

#define HEX80_RECORD_TYPE_DATA 0x00
#define HEX80_RECORD_TYPE_EOF 0x01
#define HEX80_RECORD_TYPE_EXTENDED_SEGMENT_ADDRESS 0x02
#define HEX80_RECORD_TYPE_EXTENDED_LINEAR_ADDRESS 0x04
#define HEX80_RECORD_TYPE_START_LINEAR_ADDRESS 0x05

// now we only support the data and eof type
#define is_supported_record_type(t) ((t) == HEX80_RECORD_TYPE_DATA || (t) == HEX80_RECORD_TYPE_EOF)

uint8_t calc_hex80_record_sum(const hex80_record_t& rec);

// validate and convert line to hex80 record
bool line2hex80record(std::string line, hex80_record_t& record);

// convert intel hex-80 format to vector of hex80_record_t
// return 0 for success
int hex80_to_records(const std::string& hex80_content, std::vector<hex80_record_t>& records);

typedef struct {
    uint16_t addr;
    std::vector<uint8_t> dat;
} hex80_code_snippet_t;

// merge hex80 records to snippets based on their address
void merge_hex80_records(const std::vector<hex80_record_t>& records, std::vector<hex80_code_snippet_t>& snippets);

// convert intel hex-80 format to binary
// return 0 for success
int hex80_to_bin(const std::string& hex80_content, uint8_t filler, std::vector<uint8_t>& bin_data);

// convert binary data to hex-80 string
// return 0 for success
int bin_to_hex80(const std::vector<uint8_t>& bin_data, uint8_t filler, std::string& hex80_content);

#endif /* __INTEL_HEX80_H__ */
