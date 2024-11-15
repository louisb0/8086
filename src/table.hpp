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

struct MatchCondition {
    BitField mask;
    u8 equals;

    static constexpr MatchCondition create(u8 mask, u8 equals) noexcept {
        return MatchCondition{BitField::create(mask), equals};
    }

    // NOTE(louis): this match condition will always evaluate to 'true'
    static constexpr MatchCondition none() noexcept { return MatchCondition{BitField{0, 0}, 0}; }
};

struct Encoding {
    instructions::Mnemonic mnemonic;

    MatchCondition first_byte_match;
    MatchCondition second_byte_match;

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

    static constexpr bool matches(const decode::table::Encoding &encoding, u8 first, u8 second) {
        const bool first_matches =
            encoding.first_byte_match.mask.read(first) == encoding.first_byte_match.equals;

        const bool second_matches =
            encoding.second_byte_match.mask.read(second) == encoding.second_byte_match.equals;

        return first_matches && second_matches;
    }
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
