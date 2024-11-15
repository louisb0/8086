#pragma once

#include "common.hpp"
#include "flags.hpp"
#include "instructions.hpp"
#include "registers.hpp"

#include <vector>

namespace sim::runner {

class Runner {
public:
    Runner(const std::vector<u8> instruction_memory)
        : instruction_memory(instruction_memory), regfile() {}

    void run() noexcept;

private:
    // TODO(louis): combine - good use for memory abstraction
    const std::vector<u8> instruction_memory;
    std::array<u8, 1 << 16> data_memory;

    registers::RegFile regfile;
    flags::FlagState flags;
    u16 ip = 0;

    void execute_instruction(const instructions::Instruction &inst) noexcept;

    void mov(const instructions::Instruction &inst) noexcept;
    void jump(const instructions::Instruction &inst) noexcept;
    void arithmetic(const instructions::Instruction &inst) noexcept;

    [[nodiscard]] u16 read_operand(const instructions::Operand &operand) const noexcept;
    void write_operand(const instructions::Operand &operand, u16 value) noexcept;
};

} // namespace sim::runner
