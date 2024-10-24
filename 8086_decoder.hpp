#pragma once

#include "8086.hpp"
#include "8086_matcher.hpp"

#include <fstream>
#include <optional>

// TODO(louis): Create 8086_instruction.* with a proper instruction object.
// Need good abstractions for operands for the simulator.
struct Instruction {
    std::string op;
    std::string dst;
    std::string src;

    size_t address;
    std::vector<u8> bytes;
};

class InstructionDecoder {
public:
    // TODO(louis): Understand this - is there a better way?
    InstructionDecoder(std::ifstream &file) : file(file), current_address(0) {}

    std::optional<Instruction> try_decode(const InstructionPattern &pattern,
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

    // TODO(louis): noexcept
    Instruction rm_with_reg(const InstructionPattern &pattern, u8 first);
    Instruction imm_to_rm(const InstructionPattern &pattern, u8 first);
    Instruction imm_to_reg(const InstructionPattern &pattern, u8 first);
    Instruction imm_to_acc(const InstructionPattern &pattern, u8 first);
    Instruction jump(const InstructionPattern &pattern, u8 first);

    Instruction build_instruction(std::string op, std::string dst, std::string src);

    std::string fmt_register(bool is_wide, u8 reg);
    std::string fmt_decode_rm(u8 mod, u8 rm, bool w);
};
