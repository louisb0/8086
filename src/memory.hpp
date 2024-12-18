#pragma once

#include "common.hpp"

#include "registers.hpp"

#include <string>
#include <vector>

namespace sim::mem {

struct MemoryAccess {
    registers::RegAccess terms[2];
    u16 displacement;
    bool is_wide;

    [[nodiscard]] static std::string string(const MemoryAccess &access) noexcept {
        std::string result = "[";
        bool needs_plus = false;

        for (const auto &term : access.terms) {
            if (term.index == registers::NONE)
                continue;

            if (needs_plus)
                result += " + ";

            result += registers::RegAccess::string(term);
            needs_plus = true;
        }

        if (access.displacement || !needs_plus) {
            if (needs_plus)
                result += " + ";

            result += std::to_string(access.displacement);
        }

        result += "]";
        return result;
    }
};

// TODO(louis): not good, this stuff idk should be tracked like inside the
// decoder but the decoder is also stateless so would be like a hacky reset
// thing on each iteration
//
// then we also have like the actual memory interpretation logic in runner??
//
// ODR?
class MemoryReader {
public:
    MemoryReader(const std::vector<u8> &memory, std::size_t address)
        : memory(memory), start_address(address), current_address(address) {}

    [[nodiscard]] u8 peek_byte() const noexcept { return memory.at(current_address); }

    [[nodiscard]] u8 byte() noexcept {
        u8 byte = memory.at(current_address++);
        bytes_read.push_back(byte);
        return byte;
    }

    [[nodiscard]] u16 word() noexcept {
        u16 low = byte();
        u16 high = byte();
        return (high << 8) | low;
    }

    [[nodiscard]] std::size_t get_start_address() const { return start_address; }
    [[nodiscard]] std::vector<u8> get_bytes_read() const { return bytes_read; }

private:
    const std::vector<u8> &memory;
    std::vector<u8> bytes_read;
    std::size_t start_address;
    std::size_t current_address;
};

} // namespace sim::mem
