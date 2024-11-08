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

} // namespace sim::decode
