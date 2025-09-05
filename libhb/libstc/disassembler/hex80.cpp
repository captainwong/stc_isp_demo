#include "hex80.h"

#include <assert.h>
#include <ctype.h>
#include <jlib/jlib/util/std_util.h>
#include <jlib/jlib/util/str_util.h>

uint8_t calc_hex80_record_sum(const hex80_record_t& rec) {
    uint8_t sum = 0;
    // sum += rec.st.colon; // no colon for sum
    sum += rec.st.len;
    sum += (rec.st.addr & 0xFF);  // little endian
    sum += (rec.st.addr >> 8);
    sum += rec.st.type;
    for (size_t i = 0; i < rec.st.len; i++) {
        sum += rec.st.dat[i];
    }
    return -sum;
}

bool line2hex80record(std::string line, hex80_record_t& record) {
    size_t len = line.size();
    if (len == 0 || line[0] != ':') {
        return false;
    }

    record.st.colon = line[0];
    const char* p = line.c_str();
    p++;
    len--;

    if (!read_hex(p, len, record.st.len)) {
        return false;
    }
    p += 2;
    len -= 2;
    if (len < record.st.len + 2U + 1U + 1U) {  // 2 for addr, 1 for type, 1 for sum
        return false;
    }

    if (!read_hex(p, len, record.st.addr)) {
        return false;
    }
    p += 4;
    len -= 4;

    if (!read_hex(p, len, record.st.type)) {
        return false;
    }
    p += 2;
    len -= 2;

    for (size_t i = 0; i < record.st.len; i++) {
        if (!read_hex(p, len, record.st.dat[i])) {
            return false;
        }
        p += 2;
        len -= 2;
    }

    uint8_t sum;
    if (!read_hex(p, len, sum)) {
        return false;
    }
    uint8_t mysum = calc_hex80_record_sum(record);
    if (mysum != sum) {
        return false;
    }

    return true;
}

int hex80_to_records(const std::string& hex80_content, std::vector<hex80_record_t>& records) {
    auto lines = jlib::split<std::string>(hex80_content, "\n");
    for (auto& line : lines) {
        hex80_record_t record{0};
        if (line2hex80record(line, record)) {
            records.push_back(record);
        }
    }
    return 0;
}

void merge_hex80_records(const std::vector<hex80_record_t>& records, std::vector<hex80_code_snippet_t>& snippets) {
    for (const auto& rec : records) {
        if (!is_supported_record_type(rec.st.type)) {
            continue;
        }
        if (rec.st.type == HEX80_RECORD_TYPE_EOF) {
            break;
        }
        if (snippets.empty()) {
            hex80_code_snippet_t snippet;
            snippet.addr = rec.st.addr;
            snippet.dat.insert(snippet.dat.end(), rec.st.dat, rec.st.dat + rec.st.len);
            snippets.push_back(snippet);
        } else {
            auto& last = snippets.back();
            if (last.addr + last.dat.size() == rec.st.addr) {
                last.dat.insert(last.dat.end(), rec.st.dat, rec.st.dat + rec.st.len);
            } else {
                hex80_code_snippet_t snippet;
                snippet.addr = rec.st.addr;
                snippet.dat.insert(snippet.dat.end(), rec.st.dat, rec.st.dat + rec.st.len);
                snippets.push_back(snippet);
            }
        }
    }
}

int hex80_to_bin(const std::string& hex80_content, uint8_t filler, std::vector<uint8_t>& bin_data) {
    std::vector<hex80_record_t> records;
    std::vector<hex80_code_snippet_t> snippets;
    if (hex80_to_records(hex80_content, records)) {
        return -1;
    }
    merge_hex80_records(records, snippets);
    for (const auto& snippet : snippets) {
        auto offset = snippet.addr;
        auto size = snippet.dat.size();
        if (bin_data.size() < offset) {
            // fill bin_data with filler until offset
            bin_data.resize(offset, filler);
        }
        std::copy(snippet.dat.begin(), snippet.dat.end(), bin_data.begin() + offset);
    }
    return 0;
}

int bin_to_hex80(const std::vector<uint8_t>& bin_data, uint8_t filler, std::string& hex80_content) {
    return 0;
}
