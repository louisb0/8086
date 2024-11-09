#pragma once

#include "common.hpp"

#include "instructions.hpp"

#include <vector>

namespace sim::decode::table {

struct BitField {
    u8 mask;
    u8 shift;

    constexpr u8 read(u8 byte) const noexcept { return (byte & mask) >> shift; }

    static constexpr BitField create(u8 mask) noexcept {
        u8 shift = 0;
        u8 temp_mask = mask;
        while (temp_mask && !(temp_mask & 1)) {
            shift++;
            temp_mask >>= 1;
        }
        return BitField{mask, shift};
    }
};

struct Encoding {
    instructions::Mnemonic mnemonic;

    u8 mask;
    u8 equals;

    BitField d;
    BitField s;
    BitField w;
    BitField mod;
    BitField reg;
    BitField rm;

    enum Type {
        RM_WITH_REG,
        IMM_WITH_RM,
        IMM_WITH_ACC,
        IMM_TO_REG,
        JUMP,
    } type;

    // TODO(louis): this is bad, many identical entries in the table for different encodings.
    // maybe we could abstract mask/equals into like a comparison object and we can have 1or2
    // comparisons to be true before an encoding matches
    static instructions::Mnemonic mnemonic_from_reg(u8 reg) {
        switch (reg) {
        case 0b000:
            return instructions::Mnemonic::ADD;
        case 0b101:
            return instructions::Mnemonic::SUB;
        case 0b111:
            return instructions::Mnemonic::CMP;
        default:
            UNREACHABLE();
        }
    };
};

extern const std::vector<Encoding> instruction_encodings;

// TOOD(louis): i cant find a good place to put this
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

} // namespace sim::decode::table
