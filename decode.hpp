#pragma once

#include "common.hpp"

#include "instructions.hpp"
#include "memory.hpp"
#include "table.hpp"

#include <optional>
#include <vector>

namespace sim::decode {

class Decoder {
public:
    std::optional<instructions::Instruction> try_decode(const std::vector<u8> &memory,
                                                        u8 address) noexcept;

private:
    instructions::Instruction rm_with_reg(sim::mem::MemoryReader &reader,
                                          const table::Encoding &encoding, u8 first) noexcept;
    instructions::Instruction imm_to_rm(sim::mem::MemoryReader &reader,
                                        const table::Encoding &encoding, u8 first) noexcept;
    instructions::Instruction imm_to_reg(sim::mem::MemoryReader &reader,
                                         const table::Encoding &encoding, u8 first) noexcept;
    instructions::Instruction imm_to_acc(sim::mem::MemoryReader &reader,
                                         const table::Encoding &encoding, u8 first) noexcept;
    instructions::Instruction jump(sim::mem::MemoryReader &reader,
                                   const table::Encoding &encoding) noexcept;

    instructions::Operand decode_rm(sim::mem::MemoryReader &reader, bool is_wide, u8 mod,
                                    u8 rm) noexcept;
};

struct InstructionFields {
    bool is_reg_dst;
    bool is_wide;
    bool is_sign_extended;
    u8 mod;
    u8 reg;
    u8 rm;

    static InstructionFields from(const decode::table::Encoding &encoding, u8 first) noexcept {
        return {
            .is_reg_dst = static_cast<bool>(encoding.d.read(first)),
            .is_wide = static_cast<bool>(encoding.w.read(first)),
            .is_sign_extended = static_cast<bool>(encoding.s.read(first)),
            .mod = encoding.mod.read(first),
            .reg = encoding.reg.read(first),
            .rm = encoding.rm.read(first),
        };
    }

    static InstructionFields from(const decode::table::Encoding &encoding, u8 first,
                                  u8 second) noexcept {
        return {
            .is_reg_dst = static_cast<bool>(encoding.d.read(first)),
            .is_wide = static_cast<bool>(encoding.w.read(first)),
            .is_sign_extended = static_cast<bool>(encoding.s.read(first)),
            .mod = encoding.mod.read(second),
            .reg = encoding.reg.read(second),
            .rm = encoding.rm.read(second),
        };
    }
};

} // namespace sim::decode
