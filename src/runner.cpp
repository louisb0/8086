#include "common.hpp"

#include "decode.hpp"
#include "instructions.hpp"
#include "print.hpp"
#include "runner.hpp"

#include <iostream>

namespace sim::runner {

void Runner::run() noexcept {
    while (ip < instruction_memory.size()) {
        auto inst_optional = decode::try_decode(instruction_memory, ip);
        // TODO(louis): something error handling
        if (!inst_optional) {
            break;
        }

        // TODO(louis): wtf? difference between .value()?
        const auto &inst = *inst_optional;
        ip += inst.bytes.size();

        print::instruction(inst);

        flags::FlagState before = flags;
        execute_instruction(inst);
        std::cout << flags.change_string(before) << "\n";
    }

    std::cout << regfile.string() << "\n";
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

// TODO(louis): not very elegant having to deal with immediate separately
void Runner::mov(const instructions::Instruction &inst) noexcept {
    switch (inst.src.type) {
    case instructions::Operand::Type::IMMEDIATE:
        write_operand(inst.dst, inst.src.immediate);
        break;

    case instructions::Operand::Type::MEMORY:
    case instructions::Operand::Type::REGISTER:
        write_operand(inst.dst, read_operand(inst.src));
        break;

    default:
        UNREACHABLE();
    }
}

void Runner::jump(const instructions::Instruction &inst) noexcept {
    // TODO(louis): what about 16 bit displacement? we don't store the width
    // on the immediate
    switch (inst.mnemonic) {
    case instructions::Mnemonic::JNE:
        if (!flags.test_flag(flags::Flag::ZF)) {
            ip += static_cast<int8_t>(inst.dst.immediate);
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

u16 Runner::read_operand(const instructions::Operand &operand) noexcept {
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
