#pragma once

#include "common.hpp"

#include "instructions.hpp"

#include <vector>

// quite convoluted

namespace sim::decode::table {

struct BitField {
    u8 mask;
    u8 shift;

    [[nodiscard]] constexpr u8 read(u8 byte) const noexcept { return (byte & mask) >> shift; }

    [[nodiscard]] static constexpr BitField create(u8 mask) noexcept {
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

    [[nodiscard]] static constexpr MatchCondition create(u8 mask, u8 equals) noexcept {
        return MatchCondition{BitField::create(mask), equals};
    }

    // NOTE(louis): this match condition will always evaluate to 'true'
    [[nodiscard]] static constexpr MatchCondition none() noexcept {
        return MatchCondition{BitField{0, 0}, 0};
    }
};

struct Encoding {
    instructions::Mnemonic mnemonic;

    MatchCondition opcode_pattern;
    MatchCondition reg_pattern;

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

    [[nodiscard]] static constexpr bool matches(const decode::table::Encoding &encoding, u8 first,
                                                u8 second) {
        const bool opcode_matches =
            encoding.opcode_pattern.mask.read(first) == encoding.opcode_pattern.equals;

        const bool reg_matches =
            encoding.reg_pattern.mask.read(second) == encoding.reg_pattern.equals;

        return opcode_matches && reg_matches;
    }
};

extern const std::vector<Encoding> instruction_encodings;

} // namespace sim::decode::table
