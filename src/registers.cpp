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

} // namespace sim::registers
