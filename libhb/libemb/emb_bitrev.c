#include "emb_bitrev.h"

uint8_t bitrev8(uint8_t b) {
    b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
    b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
    b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
    return b;
}

uint16_t bitrev16(uint16_t b) {
    b = (b & 0xFF00) >> 8 | (b & 0x00FF) << 8;
    b = (b & 0xF0F0) >> 4 | (b & 0x0F0F) << 4;
    b = (b & 0xCCCC) >> 2 | (b & 0x3333) << 2;
    b = (b & 0xAAAA) >> 1 | (b & 0x5555) << 1;
    return b;
}

uint32_t bitrev32(uint32_t b) {
    b = (b >> 16) | (b << 16);
    b = ((b & 0xFF00FF00) >> 8) | (b & 0x00FF00FF) << 8;
    b = ((b & 0xF0F0F0F0) >> 4) | (b & 0x0F0F0F0F) << 4;
    b = ((b & 0xCCCCCCCC) >> 2) | (b & 0x33333333) << 2;
    b = ((b & 0xAAAAAAAA) >> 1) | (b & 0x55555555) << 1;
    return b;
}
