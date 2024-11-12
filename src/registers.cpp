#include "common.hpp"

#include "registers.hpp"

#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

namespace sim::registers {

u8 &RegFile::byte_ref(u8 index) noexcept {
    u8 *bytes = reinterpret_cast<u8 *>(regs.data());
    u8 reg_num = index & 0b11;
    bool is_high = (index & 0b100) != 0;

    return bytes[2 * reg_num + (is_high ? 1 : 0)];
}

u16 RegFile::read(RegAccess access) noexcept {
    return access.is_wide ? regs[access.index] : byte_ref(access.index);
}

void RegFile::write(RegAccess access, u16 value) noexcept {
    if (access.is_wide) {
        regs[access.index] = value;
    } else {
        byte_ref(access.index) = value & 0xFF;
    }
}

std::string RegFile::string() const noexcept {
    std::stringstream ss;
    for (size_t i = 0; i < regs.size(); i++) {
        ss << std::left << std::setw(2) << REG_NAMES[i] << ": 0x" << std::right << std::hex
           << std::uppercase << std::setfill('0') << std::setw(4) << static_cast<u16>(regs[i])
           << '\n';
    }
    return ss.str();
}

// TODO(louis): fever dream please fix
std::string RegFile::format_change(const RegFile &before) const noexcept {
    std::vector<std::string> changes;
    for (size_t i = 0; i < regs.size(); i++) {
        if (regs[i] != before.regs[i]) {
            u8 curr_high = regs[i] >> 8;
            u8 curr_low = regs[i] & 0xFF;
            u8 prev_high = before.regs[i] >> 8;
            u8 prev_low = before.regs[i] & 0xFF;
            if (curr_high != prev_high && curr_low != prev_low) {
                std::stringstream ss;
                ss << std::string(REG_NAMES[i]) << " -> 0x" << std::hex << std::uppercase << regs[i]
                   << " (" << std::dec << static_cast<int16_t>(regs[i]) << ")";
                changes.push_back(ss.str());
            } else {
                if (curr_high != prev_high && i < 4) {
                    std::stringstream ss;
                    ss << std::string(REG_NAMES_HIGH[i]) << " -> 0x" << std::hex << std::uppercase
                       << static_cast<int>(curr_high) << " (" << std::dec
                       << static_cast<int8_t>(curr_high) << ")";
                    changes.push_back(ss.str());
                }
                if (curr_low != prev_low && i < 4) {
                    std::stringstream ss;
                    ss << std::string(REG_NAMES_LOW[i]) << " -> 0x" << std::hex << std::uppercase
                       << static_cast<int>(curr_low) << " (" << std::dec
                       << static_cast<int8_t>(curr_low) << ")";
                    changes.push_back(ss.str());
                }
            }
        }
    }
    std::string result;
    for (size_t i = 0; i < changes.size(); i++) {
        if (i > 0)
            result += ", ";
        result += changes[i];
    }
    return result;
}
} // namespace sim::registers
