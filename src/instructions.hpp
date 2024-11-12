#pragma once

#include "common.hpp"

#include "memory.hpp"
#include "registers.hpp"

#include <iomanip>
#include <sstream>
#include <string>
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

    [[nodiscard]] static Operand reg(u8 reg, bool is_wide) {
        return Operand{
            .type = Type::REGISTER,
            .reg_access =
                {
                    .index = reg,
                    .is_wide = is_wide,
                },
        };
    }

    [[nodiscard]] static Operand effective_address(u8 reg1, u8 reg2 = registers::NONE, u16 disp = 0,
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

    [[nodiscard]] static Operand direct_address(u16 addr, bool is_wide) {
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

    [[nodiscard]] static Operand imm(u16 imm) {
        return Operand{
            .type = Type::IMMEDIATE,
            .immediate = imm,
        };
    }

    [[nodiscard]] static Operand none() {
        return Operand{
            .type = Type::NONE,
            .immediate = 0,
        };
    }

    [[nodiscard]] static std::string string(const Operand &operand) {
        switch (operand.type) {
        case Type::REGISTER:
            return registers::RegAccess::string(operand.reg_access);
        case Type::MEMORY:
            return mem::MemoryAccess::string(operand.mem_access);
        case Type::IMMEDIATE:
            return std::to_string(operand.immediate);
        case Type::NONE:
            return "";
        default:
            UNREACHABLE();
        }
    }
};

struct Instruction {
    Mnemonic mnemonic;
    Operand dst;
    Operand src;
    size_t address;
    std::vector<u8> bytes;

    [[nodiscard]] static std::string string(const Instruction &inst) noexcept {
        std::stringstream ss;
        ss << std::hex << std::setfill('0');

        ss << std::setw(4) << inst.address << " ";

        for (u8 byte : inst.bytes) {
            ss << std::setw(2) << static_cast<int>(byte) << " ";
        }

        if (inst.bytes.size() < 6) {
            ss << std::string((6 - inst.bytes.size()) * 3, ' ');
        }

        ss << MNEMONIC_NAMES[inst.mnemonic] << " " << Operand::string(inst.dst);

        if (inst.src.type != Operand::Type::NONE) {
            ss << ", " << Operand::string(inst.src);
        }

        return ss.str();
    }
};

} // namespace sim::instructions
