#pragma once

#include "common.hpp"

#include <array>
#include <string>
#include <string_view>

namespace sim::flags {
namespace {
    static constexpr std::array<std::string_view, 2> FLAG_NAMES = {"ZF", "SF"};
}

enum Flag {
    ZF = 1 << 0,
    SF = 1 << 1,
};

class FlagState {
private:
    u16 flags = 0;

public:
    [[nodiscard]] bool test_flag(Flag f) const noexcept;
    void set_flag(Flag f, bool value) noexcept;

    [[nodiscard]] std::string change_string(FlagState before) const noexcept;
};

} // namespace sim::flags
