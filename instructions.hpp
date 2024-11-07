#pragma once

#include "common.hpp"

#include <string>
#include <vector>

namespace sim::instructions {

// TODO(louis): temporary. doesn't capture the actual way registers
// work, is incomplete, and is a weird way of doing things
namespace registers {
    static constexpr u8 BX = 3;
    static constexpr u8 BP = 5;
    static constexpr u8 SI = 6;
    static constexpr u8 DI = 7;
    static constexpr u8 NONE = 0xFF;
} // namespace registers

struct RegisterAccess {
    u8 index;
    bool is_wide;
};

struct MemoryAccess {
    RegisterAccess terms[2];
    u16 displacement;
    bool is_wide;
};

struct Operand {
    enum class Type { REGISTER, MEMORY, IMMEDIATE, NONE } type;

    union {
        RegisterAccess reg_access;
        MemoryAccess mem_access;
        u16 imm;
    };

    static Operand none() {
        return Operand{
            .type = Type::NONE,
            .imm = 0,
        };
    }

    static Operand from_register(u8 reg, bool is_wide) {
        return Operand{
            .type = Type::REGISTER,
            .reg_access =
                {
                    .index = reg,
                    .is_wide = is_wide,
                },
        };
    }

    static Operand from_immediate(u16 imm) {
        return Operand{
            .type = Type::IMMEDIATE,
            .imm = imm,
        };
    }

    // TODO(louis): readability
    static Operand effective_address(u8 reg1, u8 reg2 = 0xFF, u16 disp = 0, bool is_wide = true) {
        RegisterAccess terms[2] = {
            {reg1, true},
            {0xFF, true},
        };

        if (reg2 != 0xFF) {
            terms[1] = {reg2, true};
        }

        return Operand{
            .type = Type::MEMORY,
            .mem_access =
                {
                    .terms = {terms[0], terms[1]},
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
                    .terms = {{0xFF, true}, {0xFF, true}},
                    .displacement = addr,
                    .is_wide = is_wide,
                },
        };
    }
};

struct Instruction {
    std::string mnemonic;

    Operand dst;
    Operand src;

    size_t address;
    std::vector<u8> bytes;
};

} // namespace sim::instructions
