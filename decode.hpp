#pragma once

#include "common.hpp"

#include "instructions.hpp"
#include "table.hpp"

#include <fstream>
#include <optional>
#include <vector>

namespace sim::decode {

// TODO(louis): move? more elegant solution than function overloads?
struct InstructionFields {
    bool is_reg_dst;
    bool is_wide;
    bool is_sign_extended;
    u8 mod;
    u8 reg;
    u8 rm;

    static InstructionFields from(const table::Encoding &encoding, u8 first) noexcept {
        return {
            .is_reg_dst = static_cast<bool>(encoding.d.read(first)),
            .is_wide = static_cast<bool>(encoding.w.read(first)),
            .is_sign_extended = static_cast<bool>(encoding.s.read(first)),
            .mod = encoding.mod.read(first),
            .reg = encoding.reg.read(first),
            .rm = encoding.rm.read(first),
        };
    }

    static InstructionFields from(const table::Encoding &encoding, u8 first, u8 second) noexcept {
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

class Decoder {
public:
    Decoder(std::ifstream &file) : file(file), current_address(0) {}

    std::optional<instructions::Instruction> try_decode(const table::Encoding &encoding,
                                                        u8 byte) noexcept;

private:
    std::ifstream &file;
    size_t current_address;
    std::vector<u8> bytes;

    u8 read_byte() noexcept {
        u8 byte = file.get();
        bytes.push_back(byte);
        return byte;
    }

    u16 read_word() noexcept {
        u16 low = read_byte();
        u16 high = read_byte();
        return (high << 8) | low;
    }

    instructions::Instruction rm_with_reg(const table::Encoding &encoding, u8 first) noexcept;
    instructions::Instruction imm_to_rm(const table::Encoding &encoding, u8 first) noexcept;
    instructions::Instruction imm_to_reg(const table::Encoding &encoding, u8 first) noexcept;
    instructions::Instruction imm_to_acc(const table::Encoding &encoding, u8 first) noexcept;
    instructions::Instruction jump(const table::Encoding &encoding) noexcept;

    instructions::Operand decode_rm(bool is_wide, u8 mod, u8 rm) noexcept;
};

} // namespace sim::decode
