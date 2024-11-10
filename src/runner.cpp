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
        execute_instruction(inst);

        std::cout << "IP: 0x" << std::hex << ip << ", SF: " << static_cast<int>(test_flag(Flag::SF))
                  << ", ZF: " << static_cast<int>(test_flag(Flag::ZF)) << "\n";
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

void Runner::mov(const instructions::Instruction &inst) noexcept {
    switch (inst.src.type) {
    case instructions::Operand::Type::IMMEDIATE: {
        regfile.write(inst.dst.reg_access, inst.src.immediate);
        break;
    }
    case instructions::Operand::Type::REGISTER: {
        u16 src = regfile.read(inst.src.reg_access);
        regfile.write(inst.dst.reg_access, src);
        break;
    }
    default:
        UNREACHABLE();
    }
}

void Runner::jump(const instructions::Instruction &inst) noexcept {
    // TODO(louis): what about 16 bit displacement? we don't store the width
    // on the immediate
    switch (inst.mnemonic) {
    case instructions::Mnemonic::JNE:
        if (!test_flag(Flag::ZF)) {
            ip += static_cast<int8_t>(inst.dst.immediate);
        }

        break;
    default:
        UNREACHABLE();
    }
}

void Runner::arithmetic(const instructions::Instruction &inst) noexcept {
    u16 dst = regfile.read(inst.dst.reg_access);
    u16 src = read_source(inst);
    u16 res;

    switch (inst.mnemonic) {
    case instructions::Mnemonic::ADD:
        res = dst + src;
        regfile.write(inst.dst.reg_access, res);
        break;

    case instructions::Mnemonic::SUB:
        res = dst - src;
        regfile.write(inst.dst.reg_access, res);
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

    set_flag(Flag::ZF, res == 0);
    set_flag(Flag::SF, res & (inst.dst.reg_access.is_wide ? 0x8000 : 0x80));
}

u16 Runner::read_source(const instructions::Instruction &inst) noexcept {
    switch (inst.src.type) {
    case instructions::Operand::Type::REGISTER:
        return regfile.read(inst.src.reg_access);

    case instructions::Operand::Type::IMMEDIATE:
        return inst.src.immediate;

    default:
        UNREACHABLE();
    };
};

} // namespace sim::runner
