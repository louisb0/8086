#pragma once

#include "common.hpp"

#include <array>
#include <string>
#include <string_view>

namespace sim::registers {
namespace {
    static constexpr std::array<std::string_view, 8> REG_NAMES = {
        "ax", "cx", "dx", "bx", "sp", "bp", "si", "di",
    };
    static constexpr std::array<std::string_view, 4> REG_NAMES_HIGH = {"ah", "ch", "dh", "bh"};
    static constexpr std::array<std::string_view, 4> REG_NAMES_LOW = {"al", "cl", "dl", "bl"};
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

    [[nodiscard]] static std::string string(const RegAccess &access) noexcept {
        if (access.is_wide) {
            return std::string(REG_NAMES[access.index]);
        } else {
            return std::string((access.index & 0b100) ? REG_NAMES_HIGH[access.index & 0b11]
                                                      : REG_NAMES_LOW[access.index & 0b11]);
        }
    }
};

class RegFile {
private:
    std::array<u16, 8> regs = {};
    RegAccess recent_write; // for formatting change

public:
    [[nodiscard]] u16 read(RegAccess access) const noexcept;
    void write(RegAccess access, u16 value) noexcept;

    [[nodiscard]] std::string string() const noexcept;
    [[nodiscard]] std::string format_change(const RegFile &before) noexcept;
};

} // namespace sim::registers
