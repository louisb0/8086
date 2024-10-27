#include "common.hpp"

#include "print.hpp"

#include <iomanip>
#include <iostream>
#include <sstream>

namespace sim::print {
namespace {
    std::string format_instruction(const models::Instruction &inst) {
        std::stringstream ss;
        ss << std::hex << std::setfill('0') << std::setw(4) << inst.address << "  ";

        for (u8 byte : inst.bytes) {
            ss << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(byte)
               << " ";
        }

        for (size_t i = inst.bytes.size(); i < 6; i++) {
            ss << "   ";
        }

        ss << inst.op << " " << inst.dst;
        if (!inst.src.empty()) {
            ss << ", " << inst.src;
        }

        return ss.str();
    }
} // namespace

void instruction(const models::Instruction &inst) {
    std::cout << format_instruction(inst) << '\n';
}
} // namespace sim::print
