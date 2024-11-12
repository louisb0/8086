#pragma once

#include "common.hpp"
#include "instructions.hpp"
#include "registers.hpp"

#include <string_view>
#include <vector>

namespace sim::runner {
namespace {
    static constexpr std::array<std::string_view, 2> FLAG_NAMES = {"ZF", "SF"};
}

enum Flag {
    ZF = 1 << 0,
    SF = 1 << 1,
};

class FlagState {
private:
    u16 flags;

public:
    bool test_flag(Flag f) const noexcept { return flags & f; }
    void set_flag(Flag f, bool value) noexcept {
        if (value)
            flags |= f;
        else
            flags &= ~f;
    }

    [[nodiscard]] std::string fmt_change(FlagState before) {
        std::vector<std::string> changes;

        for (std::size_t i = 0, flag = ZF; flag <= SF; i++, flag <<= 1) {
            Flag enum_flag = static_cast<Flag>(flag);

            bool was = before.test_flag(enum_flag);
            bool is = this->test_flag(enum_flag);

            if (was != is) {
                changes.push_back(std::string(FLAG_NAMES[i]) + " -> " + (is ? "1" : "0"));
            }
        }

        std::string result;
        for (std::size_t i = 0; i < changes.size(); i++) {
            if (i > 0)
                result += ", ";

            result += changes[i];
        }

        return result;
    }
};

class Runner {
public:
    Runner(const std::vector<u8> instruction_memory)
        : instruction_memory(instruction_memory), regfile() {}

    void run() noexcept;

private:
    // TODO(louis): combine - good use for memory abstraction
    const std::vector<u8> instruction_memory;
    std::array<u8, 1 << 16> data_memory; // NOTE(louis): 16bit as no segment registers

    registers::RegFile regfile;
    FlagState flags;
    u16 ip = 0;

    void execute_instruction(const instructions::Instruction &inst) noexcept;

    void mov(const instructions::Instruction &inst) noexcept;
    void jump(const instructions::Instruction &inst) noexcept;
    void arithmetic(const instructions::Instruction &inst) noexcept;

    u16 read_operand(const instructions::Operand &operand) noexcept;
    void write_operand(const instructions::Operand &operand, u16 value) noexcept;
};

} // namespace sim::runner
