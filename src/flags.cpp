#include "flags.hpp"

#include <string>
#include <vector>

namespace sim::flags {

bool FlagState::test_flag(Flag f) const noexcept { return flags & f; }

void FlagState::set_flag(Flag f, bool value) noexcept {
    if (value)
        flags |= f;
    else
        flags &= ~f;
}

std::string FlagState::format_changes(FlagState before) const noexcept {
    std::vector<std::string> changes;

    for (std::size_t i = 0, flag = ZF; flag <= SF; i++, flag <<= 1) {
        Flag enum_flag = static_cast<Flag>(flag);

        bool was = before.test_flag(enum_flag);
        bool is = this->test_flag(enum_flag);

        if (was != is) {
            changes.push_back(std::string(FLAG_NAMES[i]) + " -> " + (is ? "1" : "0"));
        }
    }

    std::string result;
    for (std::size_t i = 0; i < changes.size(); i++) {
        if (i > 0)
            result += ", ";

        result += changes[i];
    }
    return result;
}

} // namespace sim::flags
