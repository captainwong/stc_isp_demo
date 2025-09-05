#include "intel8051is.h"

#include <jlib/jlib/util/std_util.h>
#include <jlib/jlib/util/str_util.h>

#include <list>

static std::string addr2string(uint16_t addr) {
    char buf[32];
    snprintf(buf, sizeof(buf), "%04X", addr);
    return buf;
}

static std::string opcodes2string(const uint8_t* op, size_t bytes) {
    std::string result;
    char buf[32];
    for (size_t i = 0; i < 3; ++i) {
        if (i > 0) {
            result += " ";
        }
        if (i < bytes) {
            snprintf(buf, sizeof(buf), "%02X", op[i]);
        } else {
            snprintf(buf, sizeof(buf), "  ");
        }
        result += buf;
    }
    return "[" + result + "]";
}

static std::string uint_to_string(uint32_t value) {
    char buf[32];
    snprintf(buf, sizeof(buf), "%XH", value);
    std::string out;
    if (jlib::is_contain(std::string("ABCDEF"), buf[0])) {
        out += "0";
    }
    out += buf;
    return out;
}

static std::string immed_to_string(uint32_t value) {
    return "#" + uint_to_string(value);
}

// return 0 for success
// return < 0 for fail
static int print_operands(const char* operand_str, const uint8_t* op, size_t bytes, std::string& result, size_t& offset) {
    auto operands = jlib::split<std::string>(operand_str + std::string(","), ",");
    if (operands.empty()) {
        offset++;
        return 0;
    }
    for (auto& op : operands) {
        jlib::trim(op);
    }
    std::list<std::string> output;
    size_t idx = 0;
    if (operands.size() != bytes) {
        idx++;
    }
    for (const auto& ops : operands) {
        if (ops == "addr11") {
            if (bytes < 2) {
                return E_INVALID_OPERAND;
            }
            uint16_t addr = ((op[0] >> 5) << 8) | op[1];
            output.push_back(uint_to_string(addr));
        } else if (ops == "addr16") {
            if (bytes < 3) {
                return E_INVALID_OPERAND;
            }
            uint16_t addr = (op[1] << 8) | op[2];
            output.push_back(uint_to_string(addr));
        } else if (ops == "direct" ||
                   ops == "bit" ||
                   ops == "/bit" ||
                   ops == "offset" ||
                   ops == "value") {
            if (idx >= bytes) {
                return E_INVALID_OPERAND;
            }

            // 特殊处理 F5: MOV direct, A
            if (std::string("direct, A") == operand_str) {
                idx++;
            }
            output.push_back(uint_to_string(op[idx]));
        } else if (ops == "#immed") {
            output.push_back(immed_to_string(op[idx]));
        } else {
            output.push_back(ops);
        }

        idx++;
    }
    offset += bytes;

    result += jlib::join(output, std::string(", "));
    return 0;
}

// return > 0 for ate bytes
// return < 0 for fail
static int disassemble(const uint8_t* buf, size_t len, std::string& result, size_t& offset) {
    switch (*buf) {
#define XX(op, cycle, mnem, bytes, operands)                      \
    case 0x##op:                                                  \
        if (bytes <= len) {                                       \
            int r = 0;                                            \
            std::string s = addr2string((uint16_t)offset) + ": "; \
            s += opcodes2string(buf, bytes) + " ";                \
            s += (mnem);                                          \
            s += " ";                                             \
            r = print_operands(operands, buf, bytes, s, offset);  \
            if (r < 0) {                                          \
                return r;                                         \
            }                                                     \
            s += "\n";                                            \
            result += s;                                          \
            return bytes;                                         \
        } else {                                                  \
            return E_NOT_ENOUGH;                                  \
        }
        INTEL_8051_OP_CODE_MAP(XX)
#undef XX

        default:
            printf("Unknown instruction: 0x%02X at 0x%zX\n", *buf, offset);
            return E_UNKNOWN_OP_CODE;
    }
}

// return >= 0 for success
// return < 0 for fail
int disasm(size_t addr, const uint8_t* code, size_t len, std::string& result) {
    const uint8_t* op = code;
    int ret = disassemble(op, len, result, addr);
    while (ret > 0) {
        op += ret;
        len -= ret;
        if (len == 0) {
            break;
        }

        ret = disassemble(op, len, result, addr);
    }
    return ret;
}
