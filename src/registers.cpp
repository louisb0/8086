#include "common.hpp"

#include "registers.hpp"

#include <iomanip>
#include <ios>
#include <sstream>
#include <string>

namespace sim::registers {

u16 RegFile::read(RegAccess access) const noexcept {
    if (access.is_wide) {
        return regs[access.index];
    }

    const u8 *bytes = reinterpret_cast<const u8 *>(regs.data());
    return bytes[2 * (access.index & 0b11) + ((access.is_wide) ? 0 : 1)] & 0xFF;
}

void RegFile::write(RegAccess access, u16 value) noexcept {
    if (access.is_wide) {
        regs[access.index] = value;
    } else {
        u8 *bytes = reinterpret_cast<u8 *>(regs.data());
        bytes[2 * (access.index & 0b11) + ((access.is_wide) ? 0 : 1)] = value & 0xFF;
    }

    recent_write = access;
}

std::string RegFile::string() const noexcept {
    std::stringstream ss;
    ss << std::left << std::setw(2);

    for (size_t i = 0; i < regs.size(); i++) {
        if (regs[i] == 0)
            continue;

        ss << REG_NAMES[i] << ": 0x";

        ss << std::right << std::hex << std::uppercase << std::setfill('0') << std::setw(4);
        ss << static_cast<u16>(regs[i]);

        if (i != regs.size() - 1)
            ss << "\n";
    }

    return ss.str();
}

std::string RegFile::format_change(const RegFile &before) const noexcept {
    const bool same_index = recent_write.index == before.recent_write.index;
    const bool both_wide = recent_write.is_wide && before.recent_write.is_wide;
    const bool equal_values = regs[recent_write.index] == before.regs[before.recent_write.index];

    if (same_index && both_wide && equal_values)
        return "";

    std::stringstream ss;

    ss << RegAccess::string(recent_write) << " -> ";
    ss << "0x" << std::hex << std::uppercase << regs[recent_write.index];

    ss << " (" << std::dec;
    if (recent_write.is_wide) {
        ss << static_cast<s16>(read(recent_write));
    } else {
        ss << static_cast<s8>(read(recent_write));
    }
    ss << ")";

    return ss.str();
}

} // namespace sim::registers
