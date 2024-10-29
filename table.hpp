#pragma once

#include "common.hpp"

#include <string_view>
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
    std::string_view mnemonic;

    // NOTE(louis): Replace with models::BitField?
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
        IMM_TO_RM,
        IMM_TO_ACC,
        IMM_TO_REG,
        JUMP,
    } type;
};

extern const std::vector<Encoding> instruction_encodings;

} // namespace sim::decode::table
