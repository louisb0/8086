#pragma once

#include "8086.hpp"

#include <string>
#include <vector>

namespace sim::models {
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

    // TODO(louis): create a proper abstraction. strings whole zero meaning to the system
    struct Instruction {
        std::string op;
        std::string dst;
        std::string src;

        size_t address;
        std::vector<u8> bytes;
    };
} // namespace sim::models
