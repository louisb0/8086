#pragma once

#include "common.hpp"

#include "memory.hpp"
#include "registers.hpp"
#include "table.hpp"

#include <string>
#include <vector>

namespace sim::instructions {

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

// TODO(louis): don't use string for this
struct Instruction {
    std::string mnemonic;
    Operand dst;
    Operand src;
    size_t address;
    std::vector<u8> bytes;
};

// TODO(louis): a more elegant way of doing this? repetitve and gross
struct InstructionFields {
    bool is_reg_dst;
    bool is_wide;
    bool is_sign_extended;
    u8 mod;
    u8 reg;
    u8 rm;

    static InstructionFields from(const decode::table::Encoding &encoding, u8 first) noexcept {
        return {
            .is_reg_dst = static_cast<bool>(encoding.d.read(first)),
            .is_wide = static_cast<bool>(encoding.w.read(first)),
            .is_sign_extended = static_cast<bool>(encoding.s.read(first)),
            .mod = encoding.mod.read(first),
            .reg = encoding.reg.read(first),
            .rm = encoding.rm.read(first),
        };
    }

    static InstructionFields from(const decode::table::Encoding &encoding, u8 first,
                                  u8 second) noexcept {
        return {
            .is_reg_dst = static_cast<bool>(encoding.d.read(first)),
            .is_wide = static_cast<bool>(encoding.w.read(first)),
            .is_sign_extended = static_cast<bool>(encoding.s.read(first)),
            .mod = encoding.mod.read(second),
            .reg = encoding.reg.read(second),
            .rm = encoding.rm.read(second),
        };
    }
};

} // namespace sim::instructions
