#pragma once

#include "common.hpp"

#include <array>
#include <iomanip>
#include <string_view>

namespace sim::registers {
namespace {
    static constexpr std::array<std::string_view, 8> NAMES = {
        "ax", "cx", "dx", "bx", "sp", "bp", "si", "di",
    };
    static constexpr std::array<std::string_view, 4> NAMES_HIGH = {"ah", "ch", "dh", "bh"};
    static constexpr std::array<std::string_view, 4> NAMES_LOW = {"al", "cl", "dl", "bl"};
} // namespace

enum RegIndex : u8 {
    AX = 0,
    CX,
    DX,
    BX,
    SP,
    BP,
    SI,
    DI,
    NONE,
};

static constexpr std::array<std::pair<u8, u8>, 8> EFFECTIVE_ADDRESSES = {{
    {BX, SI},
    {BX, DI},
    {BP, SI},
    {BP, DI},
    {SI, NONE},
    {DI, NONE},
    {BP, NONE},
    {BX, NONE},
}};

struct RegAccess {
    u8 index;
    bool is_wide;
};

inline std::string_view to_string(RegAccess access) {
    if (access.is_wide) {
        return NAMES[access.index];
    } else {
        return (access.index & 0b100) ? NAMES_HIGH[access.index & 0b11]
                                      : NAMES_LOW[access.index & 0b11];
    }
}

class RegFile {
private:
    std::array<u16, 8> regs = {};

    u8 &byte_ref(u8 index) noexcept {
        u8 *bytes = reinterpret_cast<u8 *>(regs.data());
        u8 reg_num = index & 0b11;
        bool is_high = (index & 0b100) != 0;
        return bytes[2 * reg_num + (is_high ? 1 : 0)];
    }

public:
    u16 read(RegAccess access) noexcept {
        return access.is_wide ? regs[access.index] : byte_ref(access.index);
    }

    void write(RegAccess access, u16 value) noexcept {
        if (access.is_wide) {
            regs[access.index] = value;
        } else {
            byte_ref(access.index) = value & 0xFF;
        }
    }

    bool test_flag(Flag f) const noexcept { return flags & f; }
    void set_flag(Flag f, bool value) noexcept {
        if (value)
            flags |= f;
        else
            flags &= ~f;
    }

    std::string string() const noexcept {
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

} // namespace sim::registers
