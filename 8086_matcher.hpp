#pragma once

#include "8086.hpp"

#include <string_view>
#include <vector>

struct BitField {
    u8 byte_relative_mask;
};

struct InstructionPattern {
    std::string_view op;

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

// TODO(louis): does this need to be a class?
class InstructionMatcher {
public:
    static const std::vector<InstructionPattern> patterns;

    static bool matches(const InstructionPattern &pattern, u8 byte) noexcept {
        return (byte & pattern.mask) == pattern.equals;
    }

    static u8 read_field(BitField field, u8 byte) {
        u8 masked = byte & field.byte_relative_mask;

        u8 shift = 0;
        u8 mask = field.byte_relative_mask;
        while (mask && !(mask & 1)) {
            shift++;
            mask >>= 1;
        }

        return masked >> shift;
    }
};
