#pragma once

#include "common.hpp"

#include <vector>

namespace sim::mem {

class MemoryReader {
public:
    MemoryReader(const std::vector<u8> &memory, std::size_t address)
        : memory(memory), start_address(address), current_address(address) {}

    u8 byte() noexcept {
        u8 byte = memory.at(current_address++);
        bytes_read.push_back(byte);
        return byte;
    }

    u16 word() noexcept {
        u16 low = byte();
        u16 high = byte();
        return (high << 8) | low;
    }

    std::size_t get_start_address() const { return start_address; }
    std::vector<u8> get_bytes_read() const { return bytes_read; }

private:
    const std::vector<u8> &memory;
    std::vector<u8> bytes_read;
    std::size_t start_address;
    std::size_t current_address;
};

} // namespace sim::mem
