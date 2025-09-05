#ifndef __HEX_H__
#define __HEX_H__

#include <assert.h>
#include <stdint.h>

#include <string>
#include <vector>

// xdigit to decimal
inline uint8_t x2d(uint8_t ascii) {
    if ('0' <= ascii && ascii <= '9') {
        return ascii - '0';
    }
    if ('A' <= ascii && ascii <= 'F') {
        return ascii - 'A' + 10;
    }
    if ('a' <= ascii && ascii <= 'f') {
        return ascii - 'a' + 10;
    }
    assert(false);
    return 0;
}

template <typename value_type = uint8_t>
inline bool read_hex(const char* str, size_t len, value_type& hex) {
    if (len < sizeof(value_type) * 2) {
        return false;
    }
    hex = 0;
    for (size_t i = 0; i < sizeof(value_type) * 2; i++) {
        if (!isxdigit(str[i])) {
            return false;
        } else {
            hex = (hex << 4) | x2d(str[i]);
        }
    }
    return true;
}

// convert ascii-hex format to binary
// the ascii-hex can contains spaces as separators
// return 0 for success
int hex_to_bin(const std::string& hex_content, std::vector<uint8_t>& bin_data);

#endif /* __HEX_H__ */
