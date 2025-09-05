#include "hex.h"

#include <jlib/jlib/util/std_util.h>
#include <jlib/jlib/util/str_util.h>

int hex_to_bin(const std::string& hex_content, std::vector<uint8_t>& bin_data) {
    auto hex = jlib::erase_all_copy(hex_content, ' ');
    jlib::erase_all(hex, '\t');
    jlib::erase_all(hex, '\r');
    jlib::erase_all(hex, '\n');

    bin_data.clear();
    if (hex.empty() || (hex.length() % 2 != 0)) {
        return 0;
    }

    const char* p = hex.c_str();
    size_t len = hex.size();
    while (len > 0) {
        uint8_t byte = 0;
        if (!read_hex(p, len, byte)) {
            return -1;
        }
        bin_data.push_back(byte);
        p += 2;
        len -= 2;
    }
    return 0;
}
