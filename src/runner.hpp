#pragma once

#include "common.hpp"
#include "instructions.hpp"
#include "registers.hpp"

#include <vector>

namespace sim::runner {

enum Flag {
    ZF = 1 << 0,
    SF = 1 << 1,
};

class Runner {
public:
    Runner(const std::vector<u8> instruction_memory)
        : instruction_memory(instruction_memory), regfile() {}

    void run() noexcept;

private:
    const std::vector<u8> instruction_memory;
    registers::RegFile regfile;
    u16 flags = 0;
    u16 ip = 0;

    void execute_instruction(const instructions::Instruction &inst) noexcept;

    void mov(const instructions::Instruction &inst) noexcept;
    void jump(const instructions::Instruction &inst) noexcept;
    void arithmetic(const instructions::Instruction &inst) noexcept;

    u16 read_source(const sim::instructions::Instruction &inst) noexcept;

    bool test_flag(Flag f) const noexcept { return flags & f; }
    void set_flag(Flag f, bool value) noexcept {
        if (value)
            flags |= f;
        else
            flags &= ~f;
    }
};

} // namespace sim::runner
