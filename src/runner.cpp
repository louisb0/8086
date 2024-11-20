#include "common.hpp"

#include "decode.hpp"
#include "instructions.hpp"
#include "registers.hpp"
#include "runner.hpp"

#include <iostream>

namespace sim::runner {

void Runner::run() noexcept {
    // NOTE(louis): This is inefficient, trying the whole table until we get a hit.
    // We could pre-compute a jump table or something, but I want to move onto another project :)
    while (ip < instruction_memory.size()) {
        const auto inst_optional = decode::try_decode(instruction_memory, ip);
        if (!inst_optional) {
            std::cerr << "failed to decode instruction at 0x" << std::hex << ip << "\n";
            break;
        }

        const auto &inst = inst_optional.value();
        ip += inst.bytes.size();

        const auto regfile_before = regfile;
        const auto flags_before = flags;

        execute_instruction(inst);

        const auto reg_changes = regfile.format_change(regfile_before);
        const auto flag_changes = flags.format_changes(flags_before);

        std::cout << std::left << std::setw(40) << instructions::Instruction::string(inst);

        if (!reg_changes.empty()) {
            std::cout << "| r[" << reg_changes << "]";
            if (!flag_changes.empty()) {
                std::cout << ", f[" << flag_changes << "]";
            }
        }
        std::cout << '\n';
    }

    std::cout << '\n' << regfile.string();
}

void Runner::execute_instruction(const instructions::Instruction &inst) noexcept {
    switch (inst.mnemonic) {
    case instructions::Mnemonic::MOV:
        mov(inst);
        break;

    case instructions::Mnemonic::JNE:
        jump(inst);
        break;

    case instructions::Mnemonic::ADD:
    case instructions::Mnemonic::SUB:
    case instructions::Mnemonic::CMP:
        arithmetic(inst);
        break;

    default:
        UNREACHABLE();
    }
}

void Runner::mov(const instructions::Instruction &inst) noexcept {
    write_operand(inst.dst, read_operand(inst.src));
}

void Runner::jump(const instructions::Instruction &inst) noexcept {
    switch (inst.mnemonic) {
    case instructions::Mnemonic::JNE:
        if (!flags.test_flag(flags::Flag::ZF)) {
            ip += static_cast<s8>(inst.dst.immediate);
        }

        break;
    default:
        UNREACHABLE();
    }
}

void Runner::arithmetic(const instructions::Instruction &inst) noexcept {
    u16 dst = read_operand(inst.dst);
    u16 src = read_operand(inst.src);
    u16 res;

    switch (inst.mnemonic) {
    case instructions::Mnemonic::ADD:
        res = dst + src;
        write_operand(inst.dst, res);
        break;

    case instructions::Mnemonic::SUB:
        res = dst - src;
        write_operand(inst.dst, res);
        break;

    case instructions::Mnemonic::CMP:
        res = dst - src;
        break;

    default:
        UNREACHABLE();
    }

    if (!inst.dst.reg_access.is_wide) {
        res &= 0xFF;
    }

    flags.set_flag(flags::Flag::ZF, res == 0);
    flags.set_flag(flags::Flag::SF, res & (inst.dst.reg_access.is_wide ? 0x8000 : 0x80));
}

u16 Runner::read_operand(const instructions::Operand &operand) const noexcept {
    switch (operand.type) {
    case instructions::Operand::Type::REGISTER:
        return regfile.read(operand.reg_access);

    case instructions::Operand::Type::IMMEDIATE:
        return operand.immediate;

    case instructions::Operand::Type::MEMORY: {
        u16 offset = operand.mem_access.displacement;

        if (operand.mem_access.terms[0].index != registers::NONE)
            offset += regfile.read(operand.mem_access.terms[0]);

        if (operand.mem_access.terms[1].index != registers::NONE)
            offset += regfile.read(operand.mem_access.terms[1]);

        if (operand.mem_access.is_wide) {
            u8 low = data_memory[offset];
            u8 high = data_memory[offset + 1];

            return (high << 8) | low;
        } else {
            return data_memory[offset];
        }
    }

    default:
        UNREACHABLE();
    }
}

void Runner::write_operand(const instructions::Operand &operand, u16 value) noexcept {
    switch (operand.type) {
    case instructions::Operand::Type::REGISTER:
        return regfile.write(operand.reg_access, value);

    case instructions::Operand::Type::MEMORY: {
        u16 offset = operand.mem_access.displacement;

        if (operand.mem_access.terms[0].index != registers::NONE)
            offset += regfile.read(operand.mem_access.terms[0]);

        if (operand.mem_access.terms[1].index != registers::NONE)
            offset += regfile.read(operand.mem_access.terms[1]);

        if (operand.mem_access.is_wide) {
            data_memory[offset] = value & 0xFF;
            data_memory[offset + 1] = (value >> 8);
        } else {
            data_memory[offset] = value & 0xFF;
        }

        break;
    }

    default:
        UNREACHABLE();
    }
}

} // namespace sim::runner
