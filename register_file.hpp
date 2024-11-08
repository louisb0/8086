#pragma once

#include "common.hpp"

#include "instructions.hpp"

#include <array>
#include <iomanip>

namespace sim::simulator {
namespace {
    struct Flags {
        u8 ZF : 1;
        u8 SF : 1;
    };
} // namespace

// TODO(louis): hpp/cpp split?
class RegisterFile {
private:
    std::array<u16, 8> regs = {};

    u8 &byte_ref(u8 index) noexcept {
        u8 *bytes = reinterpret_cast<u8 *>(regs.data());
        u8 reg_num = index & 0b11;
        bool is_high = (index & 0b100) != 0;
        return bytes[2 * reg_num + (is_high ? 1 : 0)];
    }

public:
    Flags flags = {.ZF = 0, .SF = 0};

    u16 read(instructions::RegisterAccess access) {
        return access.is_wide ? regs[access.index] : byte_ref(access.index);
    }

    void write(instructions::RegisterAccess access, u16 value) {
        if (access.is_wide) {
            regs[access.index] = value;
        } else {
            byte_ref(access.index) = value & 0xFF;
        }
    }

    std::string string() {
        std::stringstream ss;
        static constexpr std::string_view reg_names[] = {"ax", "cx", "dx", "bx",
                                                         "sp", "bp", "si", "di"};

        for (size_t i = 0; i < regs.size(); i++) {
            ss << std::left << std::setw(2) << reg_names[i] << ": 0x" << std::right << std::hex
               << std::uppercase << std::setfill('0') << std::setw(4) << static_cast<u16>(regs[i])
               << '\n';
        }

        return ss.str();
    }
};

}; // namespace sim::simulator
