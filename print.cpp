#include "common.hpp"

#include "instructions.hpp"
#include "memory.hpp"
#include "print.hpp"
#include "registers.hpp"

#include <iomanip>
#include <iostream>
#include <sstream>

namespace sim::print {

namespace {
    static const char *WORD_REGISTERS[] = {"ax", "cx", "dx", "bx", "sp", "bp", "si", "di"};
    static const char *BYTE_REGISTERS[] = {"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"};

    std::string format_register(const registers::RegAccess &reg) noexcept {
        return reg.is_wide ? WORD_REGISTERS[reg.index] : BYTE_REGISTERS[reg.index];
    }

    std::string format_immediate(u16 imm) noexcept {
        std::stringstream ss;
        ss << std::dec << static_cast<u16>(imm);
        return ss.str();
    }

    std::string format_memory(const mem::MemoryAccess &mem) noexcept {
        std::stringstream ss;
        ss << "[";

        bool needs_plus = false;
        for (const registers::RegAccess term : mem.terms) {
            if (term.index == registers::NONE)
                continue;

            if (needs_plus)
                ss << " + ";

            ss << format_register(term);
            needs_plus = true;
        }

        if (mem.displacement != 0) {
            if (needs_plus)
                ss << " + ";

            ss << std::dec << static_cast<int16_t>(mem.displacement);
        }

        ss << "]";
        return ss.str();
    }

    std::string format_operand(const instructions::Operand &op) noexcept {
        switch (op.type) {
        case instructions::Operand::Type::REGISTER:
            return format_register(op.reg_access);
        case instructions::Operand::Type::MEMORY:
            return format_memory(op.mem_access);
        case instructions::Operand::Type::IMMEDIATE:
            return format_immediate(op.immediate);
        default:
            return "???";
        }
    }

    std::string format_instruction(const instructions::Instruction &inst) noexcept {
        std::stringstream ss;

        ss << std::hex << std::setfill('0') << std::setw(4) << inst.address << "  ";

        for (u8 byte : inst.bytes) {
            ss << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(byte) << " ";
        }

        for (size_t i = inst.bytes.size(); i < 6; i++) {
            ss << "   ";
        }

        ss << instructions::MNEMONIC_NAMES[inst.mnemonic] << " " << format_operand(inst.dst);

        if (inst.src.type != instructions::Operand::Type::NONE) {
            ss << ", " << format_operand(inst.src);
        }

        return ss.str();
    }
} // namespace

void instruction(const instructions::Instruction &inst) noexcept {
    std::cout << format_instruction(inst) << '\n';
}

} // namespace sim::print
