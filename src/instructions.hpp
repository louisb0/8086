#pragma once

#include "common.hpp"

#include "memory.hpp"
#include "registers.hpp"

#include <vector>

namespace sim::instructions {

static constexpr std::array<std::string_view, 24> MNEMONIC_NAMES = {
    "mov", "add", "sub", "cmp", "je", "jl",  "jle", "jb",  "jbe",  "jp",    "jo",     "js",
    "jne", "jnl", "jg",  "jnb", "ja", "jnp", "jno", "jns", "loop", "loopz", "loopnz", "jcxz"};

enum Mnemonic : u8 {
    MOV,
    ADD,
    SUB,
    CMP,
    JE,
    JL,
    JLE,
    JB,
    JBE,
    JP,
    JO,
    JS,
    JNE,
    JNL,
    JG,
    JNB,
    JA,
    JNP,
    JNO,
    JNS,
    LOOP,
    LOOPZ,
    LOOPNZ,
    JCXZ
};

struct Operand {
    enum class Type { REGISTER, MEMORY, IMMEDIATE, NONE } type;

    union {
        registers::RegAccess reg_access;
        mem::MemoryAccess mem_access;
        u16 immediate;
    };

    static Operand none() {
        return Operand{
            .type = Type::NONE,
            .immediate = 0,
        };
    }

    static Operand imm(u16 imm) {
        return Operand{
            .type = Type::IMMEDIATE,
            .immediate = imm,
        };
    }

    static Operand reg(u8 reg, bool is_wide) {
        return Operand{
            .type = Type::REGISTER,
            .reg_access =
                {
                    .index = reg,
                    .is_wide = is_wide,
                },
        };
    }

    static Operand effective_address(u8 reg1, u8 reg2 = registers::NONE, u16 disp = 0,
                                     bool is_wide = true) {
        return Operand{
            .type = Type::MEMORY,
            .mem_access =
                {
                    .terms = {{reg1, true}, {reg2, true}},
                    .displacement = disp,
                    .is_wide = is_wide,
                },
        };
    }

    static Operand direct_address(u16 addr, bool is_wide) {
        return Operand{
            .type = Type::MEMORY,
            .mem_access =
                {
                    .terms = {{registers::NONE, true}, {registers::NONE, true}},
                    .displacement = addr,
                    .is_wide = is_wide,
                },
        };
    }
};

struct Instruction {
    Mnemonic mnemonic;
    Operand dst;
    Operand src;
    size_t address;
    std::vector<u8> bytes;
};

} // namespace sim::instructions
