#ifndef __INTEL_8051_INSTRUCTION_SET_H__
#define __INTEL_8051_INSTRUCTION_SET_H__

#include <stdint.h>

#include <string>

// 8051 instructions
#define INTEL_8051_OP_CODE_MAP(XX)              \
    /* OP CYCLE MNEM BYTES OPERANDS */          \
    XX(00, 1, "NOP", 1, "")                     \
    XX(01, 2, "AJMP", 2, "addr11")              \
    XX(02, 2, "LJMP", 3, "addr16")              \
    XX(03, 1, "RR", 1, "A")                     \
    XX(04, 1, "INC", 1, "A")                    \
    XX(05, 2, "INC", 2, "direct")               \
    XX(06, 1, "INC", 1, "@R0")                  \
    XX(07, 1, "INC", 1, "@R1")                  \
    XX(08, 1, "INC", 1, "R0")                   \
    XX(09, 1, "INC", 1, "R1")                   \
    XX(0A, 1, "INC", 1, "R2")                   \
    XX(0B, 1, "INC", 1, "R3")                   \
    XX(0C, 1, "INC", 1, "R4")                   \
    XX(0D, 1, "INC", 1, "R5")                   \
    XX(0E, 1, "INC", 1, "R6")                   \
    XX(0F, 1, "INC", 1, "R7")                   \
    XX(10, 2, "JBC", 3, "bit, offset")          \
    XX(11, 2, "ACALL", 2, "addr11")             \
    XX(12, 2, "LCALL", 3, "addr16")             \
    XX(13, 1, "RRC", 1, "A")                    \
    XX(14, 1, "DEC", 1, "A")                    \
    XX(15, 2, "DEC", 2, "direct")               \
    XX(16, 1, "DEC", 1, "@R0")                  \
    XX(17, 1, "DEC", 1, "@R1")                  \
    XX(18, 1, "DEC", 1, "R0")                   \
    XX(19, 1, "DEC", 1, "R1")                   \
    XX(1A, 1, "DEC", 1, "R2")                   \
    XX(1B, 1, "DEC", 1, "R3")                   \
    XX(1C, 1, "DEC", 1, "R4")                   \
    XX(1D, 1, "DEC", 1, "R5")                   \
    XX(1E, 1, "DEC", 1, "R6")                   \
    XX(1F, 1, "DEC", 1, "R7")                   \
    XX(20, 2, "JB", 3, "bit, offset")           \
    XX(21, 2, "AJMP", 2, "addr11")              \
    XX(22, 2, "RET", 1, "")                     \
    XX(23, 1, "RL", 1, "A")                     \
    XX(24, 1, "ADD", 2, "A, #immed")            \
    XX(25, 1, "ADD", 2, "A, direct")            \
    XX(26, 1, "ADD", 1, "A, @R0")               \
    XX(27, 1, "ADD", 1, "A, @R1")               \
    XX(28, 1, "ADD", 1, "A, R0")                \
    XX(29, 1, "ADD", 1, "A, R1")                \
    XX(2A, 1, "ADD", 1, "A, R2")                \
    XX(2B, 1, "ADD", 1, "A, R3")                \
    XX(2C, 1, "ADD", 1, "A, R4")                \
    XX(2D, 1, "ADD", 1, "A, R5")                \
    XX(2E, 1, "ADD", 1, "A, R6")                \
    XX(2F, 1, "ADD", 1, "A, R7")                \
    XX(30, 2, "JNB", 3, "bit, offset")          \
    XX(31, 2, "ACALL", 2, "addr11")             \
    XX(32, 2, "RETI", 1, "")                    \
    XX(33, 1, "RLC", 1, "A")                    \
    XX(34, 1, "ADDC", 2, "A, #immed")           \
    XX(35, 1, "ADDC", 2, "A, direct")           \
    XX(36, 1, "ADDC", 1, "A, @R0")              \
    XX(37, 1, "ADDC", 1, "A, @R1")              \
    XX(38, 1, "ADDC", 1, "A, R0")               \
    XX(39, 1, "ADDC", 1, "A, R1")               \
    XX(3A, 1, "ADDC", 1, "A, R2")               \
    XX(3B, 1, "ADDC", 1, "A, R3")               \
    XX(3C, 1, "ADDC", 1, "A, R4")               \
    XX(3D, 1, "ADDC", 1, "A, R5")               \
    XX(3E, 1, "ADDC", 1, "A, R6")               \
    XX(3F, 1, "ADDC", 1, "A, R7")               \
    XX(40, 2, "JC", 2, "offset")                \
    XX(41, 2, "AJMP", 2, "addr11")              \
    XX(42, 1, "ORL", 2, "direct, A")            \
    XX(43, 2, "ORL", 3, "direct, #immed")       \
    XX(44, 1, "ORL", 2, "A, #immed")            \
    XX(45, 1, "ORL", 2, "A, direct")            \
    XX(46, 1, "ORL", 1, "A, @R0")               \
    XX(47, 1, "ORL", 1, "A, @R1")               \
    XX(48, 1, "ORL", 1, "A, R0")                \
    XX(49, 1, "ORL", 1, "A, R1")                \
    XX(4A, 1, "ORL", 1, "A, R2")                \
    XX(4B, 1, "ORL", 1, "A, R3")                \
    XX(4C, 1, "ORL", 1, "A, R4")                \
    XX(4D, 1, "ORL", 1, "A, R5")                \
    XX(4E, 1, "ORL", 1, "A, R6")                \
    XX(4F, 1, "ORL", 1, "A, R7")                \
    XX(50, 2, "JNC", 2, "offset")               \
    XX(51, 2, "ACALL", 2, "addr11")             \
    XX(52, 1, "ANL", 2, "direct, A")            \
    XX(53, 2, "ANL", 3, "direct, #immed")       \
    XX(54, 1, "ANL", 2, "A, #immed")            \
    XX(55, 1, "ANL", 2, "A, direct")            \
    XX(56, 1, "ANL", 1, "A, @R0")               \
    XX(57, 1, "ANL", 1, "A, @R1")               \
    XX(58, 1, "ANL", 1, "A, R0")                \
    XX(59, 1, "ANL", 1, "A, R1")                \
    XX(5A, 1, "ANL", 1, "A, R2")                \
    XX(5B, 1, "ANL", 1, "A, R3")                \
    XX(5C, 1, "ANL", 1, "A, R4")                \
    XX(5D, 1, "ANL", 1, "A, R5")                \
    XX(5E, 1, "ANL", 1, "A, R6")                \
    XX(5F, 1, "ANL", 1, "A, R7")                \
    XX(60, 2, "JZ", 2, "offset")                \
    XX(61, 2, "AJMP", 2, "addr11")              \
    XX(62, 1, "XRL", 2, "direct, A")            \
    XX(63, 2, "XRL", 3, "direct, #immed")       \
    XX(64, 1, "XRL", 2, "A, #immed")            \
    XX(65, 1, "XRL", 2, "A, direct")            \
    XX(66, 1, "XRL", 1, "A, @R0")               \
    XX(67, 1, "XRL", 1, "A, @R1")               \
    XX(68, 1, "XRL", 1, "A, R0")                \
    XX(69, 1, "XRL", 1, "A, R1")                \
    XX(6A, 1, "XRL", 1, "A, R2")                \
    XX(6B, 1, "XRL", 1, "A, R3")                \
    XX(6C, 1, "XRL", 1, "A, R4")                \
    XX(6D, 1, "XRL", 1, "A, R5")                \
    XX(6E, 1, "XRL", 1, "A, R6")                \
    XX(6F, 1, "XRL", 1, "A, R7")                \
    XX(70, 2, "JNZ", 2, "offset")               \
    XX(71, 2, "ACALL", 2, "addr11")             \
    XX(72, 2, "ORL", 2, "C, bit")               \
    XX(73, 2, "JMP", 1, "@A+DPTR")              \
    XX(74, 1, "MOV", 2, "A, #immed")            \
    XX(75, 2, "MOV", 3, "direct, #immed")       \
    XX(76, 1, "MOV", 2, "@R0, #immed")          \
    XX(77, 1, "MOV", 2, "@R1, #immed")          \
    XX(78, 1, "MOV", 2, "R0, #immed")           \
    XX(79, 1, "MOV", 2, "R1, #immed")           \
    XX(7A, 1, "MOV", 2, "R2, #immed")           \
    XX(7B, 1, "MOV", 2, "R3, #immed")           \
    XX(7C, 1, "MOV", 2, "R4, #immed")           \
    XX(7D, 1, "MOV", 2, "R5, #immed")           \
    XX(7E, 1, "MOV", 2, "R6, #immed")           \
    XX(7F, 1, "MOV", 2, "R7, #immed")           \
    XX(80, 2, "SJMP", 2, "offset")              \
    XX(81, 2, "AJMP", 2, "addr11")              \
    XX(82, 2, "ANL", 2, "C, bit")               \
    XX(83, 2, "MOVC", 1, "A, @A+PC")            \
    XX(84, 4, "DIV", 1, "AB")                   \
    XX(85, 2, "MOV", 3, "direct, direct")       \
    XX(86, 2, "MOV", 2, "direct, @R0")          \
    XX(87, 2, "MOV", 2, "direct, @R1")          \
    XX(88, 2, "MOV", 2, "direct, R0")           \
    XX(89, 2, "MOV", 2, "direct, R1")           \
    XX(8A, 2, "MOV", 2, "direct, R2")           \
    XX(8B, 2, "MOV", 2, "direct, R3")           \
    XX(8C, 2, "MOV", 2, "direct, R4")           \
    XX(8D, 2, "MOV", 2, "direct, R5")           \
    XX(8E, 2, "MOV", 2, "direct, R6")           \
    XX(8F, 2, "MOV", 2, "direct, R7")           \
    XX(90, 2, "MOV", 3, "DPTR, #immed")         \
    XX(91, 2, "ACALL", 2, "addr11")             \
    XX(92, 2, "MOV", 2, "bit, C")               \
    XX(93, 2, "MOVC", 1, "A, @A+DPTR")          \
    XX(94, 1, "SUBB", 2, "A, #immed")           \
    XX(95, 1, "SUBB", 2, "A, direct")           \
    XX(96, 1, "SUBB", 1, "A, @R0")              \
    XX(97, 1, "SUBB", 1, "A, @R1")              \
    XX(98, 1, "SUBB", 1, "A, R0")               \
    XX(99, 1, "SUBB", 1, "A, R1")               \
    XX(9A, 1, "SUBB", 1, "A, R2")               \
    XX(9B, 1, "SUBB", 1, "A, R3")               \
    XX(9C, 1, "SUBB", 1, "A, R4")               \
    XX(9D, 1, "SUBB", 1, "A, R5")               \
    XX(9E, 1, "SUBB", 1, "A, R6")               \
    XX(9F, 1, "SUBB", 1, "A, R7")               \
    XX(A0, 2, "ORL", 2, "C, /bit")              \
    XX(A1, 2, "AJMP", 2, "addr11")              \
    XX(A2, 1, "MOV", 2, "C, bit")               \
    XX(A3, 2, "INC", 1, "DPTR")                 \
    XX(A4, 4, "MUL", 1, "AB")                   \
    /* A5 reserved */                           \
    XX(A5, 2, "DB", 2, "value")                 \
    XX(A6, 2, "MOV", 2, "@R0, direct")          \
    XX(A7, 2, "MOV", 2, "@R1, direct")          \
    XX(A8, 2, "MOV", 2, "R0, direct")           \
    XX(A9, 2, "MOV", 2, "R1, direct")           \
    XX(AA, 2, "MOV", 2, "R2, direct")           \
    XX(AB, 2, "MOV", 2, "R3, direct")           \
    XX(AC, 2, "MOV", 2, "R4, direct")           \
    XX(AD, 2, "MOV", 2, "R5, direct")           \
    XX(AE, 2, "MOV", 2, "R6, direct")           \
    XX(AF, 2, "MOV", 2, "R7, direct")           \
    XX(B0, 2, "ANL", 2, "C, /bit")              \
    XX(B1, 2, "ACALL", 2, "addr11")             \
    XX(B2, 1, "CPL", 2, "bit")                  \
    XX(B3, 1, "CPL", 1, "C")                    \
    XX(B4, 2, "CJNE", 3, "A, #immed, offset")   \
    XX(B5, 2, "CJNE", 3, "A, direct, offset")   \
    XX(B6, 2, "CJNE", 3, "@R0, #immed, offset") \
    XX(B7, 2, "CJNE", 3, "@R1, #immed, offset") \
    XX(B8, 2, "CJNE", 3, "R0, #immed, offset")  \
    XX(B9, 2, "CJNE", 3, "R1, #immed, offset")  \
    XX(BA, 2, "CJNE", 3, "R2, #immed, offset")  \
    XX(BB, 2, "CJNE", 3, "R3, #immed, offset")  \
    XX(BC, 2, "CJNE", 3, "R4, #immed, offset")  \
    XX(BD, 2, "CJNE", 3, "R5, #immed, offset")  \
    XX(BE, 2, "CJNE", 3, "R6, #immed, offset")  \
    XX(BF, 2, "CJNE", 3, "R7, #immed, offset")  \
    XX(C0, 2, "PUSH", 2, "direct")              \
    XX(C1, 2, "AJMP", 2, "addr11")              \
    XX(C2, 1, "CLR", 2, "bit")                  \
    XX(C3, 1, "CLR", 1, "C")                    \
    XX(C4, 1, "SWAP", 1, "A")                   \
    XX(C5, 1, "XCH", 2, "A, direct")            \
    XX(C6, 1, "XCH", 1, "A, @R0")               \
    XX(C7, 1, "XCH", 1, "A, @R1")               \
    XX(C8, 1, "XCH", 1, "A, R0")                \
    XX(C9, 1, "XCH", 1, "A, R1")                \
    XX(CA, 1, "XCH", 1, "A, R2")                \
    XX(CB, 1, "XCH", 1, "A, R3")                \
    XX(CC, 1, "XCH", 1, "A, R4")                \
    XX(CD, 1, "XCH", 1, "A, R5")                \
    XX(CE, 1, "XCH", 1, "A, R6")                \
    XX(CF, 1, "XCH", 1, "A, R7")                \
    XX(D0, 2, "POP", 2, "direct")               \
    XX(D1, 2, "ACALL", 2, "addr11")             \
    XX(D2, 1, "SETB", 2, "bit")                 \
    XX(D3, 1, "SETB", 1, "C")                   \
    XX(D4, 1, "DA", 1, "A")                     \
    XX(D5, 2, "DJNZ", 3, "direct, offset")      \
    XX(D6, 1, "XCHD", 1, "A, @R0")              \
    XX(D7, 1, "XCHD", 1, "A, @R1")              \
    XX(D8, 2, "DJNZ", 2, "R0, offset")          \
    XX(D9, 2, "DJNZ", 2, "R1, offset")          \
    XX(DA, 2, "DJNZ", 2, "R2, offset")          \
    XX(DB, 2, "DJNZ", 2, "R3, offset")          \
    XX(DC, 2, "DJNZ", 2, "R4, offset")          \
    XX(DD, 2, "DJNZ", 2, "R5, offset")          \
    XX(DE, 2, "DJNZ", 2, "R6, offset")          \
    XX(DF, 2, "DJNZ", 2, "R7, offset")          \
    XX(E0, 2, "MOVX", 1, "A, @DPTR")            \
    XX(E1, 2, "AJMP", 2, "addr11")              \
    XX(E2, 2, "MOVX", 1, "A, @R0")              \
    XX(E3, 2, "MOVX", 1, "A, @R1")              \
    XX(E4, 1, "CLR", 1, "A")                    \
    XX(E5, 1, "MOV", 2, "A, direct")            \
    XX(E6, 1, "MOV", 1, "A, @R0")               \
    XX(E7, 1, "MOV", 1, "A, @R1")               \
    XX(E8, 1, "MOV", 1, "A, R0")                \
    XX(E9, 1, "MOV", 1, "A, R1")                \
    XX(EA, 1, "MOV", 1, "A, R2")                \
    XX(EB, 1, "MOV", 1, "A, R3")                \
    XX(EC, 1, "MOV", 1, "A, R4")                \
    XX(ED, 1, "MOV", 1, "A, R5")                \
    XX(EE, 1, "MOV", 1, "A, R6")                \
    XX(EF, 1, "MOV", 1, "A, R7")                \
    XX(F0, 2, "MOVX", 1, "@DPTR, A")            \
    XX(F1, 2, "ACALL", 2, "addr11")             \
    XX(F2, 2, "MOVX", 1, "@R0, A")              \
    XX(F3, 2, "MOVX", 1, "@R1, A")              \
    XX(F4, 1, "CPL", 1, "A")                    \
    XX(F5, 1, "MOV", 2, "direct, A")            \
    XX(F6, 1, "MOV", 1, "@R0, A")               \
    XX(F7, 1, "MOV", 1, "@R1, A")               \
    XX(F8, 1, "MOV", 1, "R0, A")                \
    XX(F9, 1, "MOV", 1, "R1, A")                \
    XX(FA, 1, "MOV", 1, "R2, A")                \
    XX(FB, 1, "MOV", 1, "R3, A")                \
    XX(FC, 1, "MOV", 1, "R4, A")                \
    XX(FD, 1, "MOV", 1, "R5, A")                \
    XX(FE, 1, "MOV", 1, "R6, A")                \
    XX(FF, 1, "MOV", 1, "R7, A")

#define E_UNKNOWN_OP_CODE -1
#define E_NOT_ENOUGH -2
#define E_INVALID_OPERAND -3

/**
 * Disassemble Intel 8051 instructions.
 * @param addr The starting address of the instruction.
 * @param code The instruction bytecode.
 * @param len The length of the instruction bytecode.
 * @param result The disassembled instruction string.
 * @return The number of bytes disassembled, or a negative error code.
 */
int disasm(size_t addr, const uint8_t* code, size_t len, std::string& result);

#endif /* __INTEL_8051_INSTRUCTION_SET_H__ */
