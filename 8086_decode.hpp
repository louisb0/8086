#pragma once

#include "8086.hpp"

#include "8086_models.hpp"
#include "8086_table.hpp"

#include <fstream>
#include <optional>
#include <string>
#include <vector>

namespace sim::decode {

class Decoder {
public:
    Decoder(std::ifstream &file) : file(file), current_address(0) {}

    std::optional<models::Instruction> try_decode(const table::Encoding &pattern,
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

    models::Instruction rm_with_reg(const table::Encoding &pattern, u8 first) noexcept;
    models::Instruction imm_to_rm(const table::Encoding &pattern, u8 first) noexcept;
    models::Instruction imm_to_reg(const table::Encoding &pattern, u8 first) noexcept;
    models::Instruction imm_to_acc(const table::Encoding &pattern, u8 first) noexcept;
    models::Instruction jump(const table::Encoding &pattern, u8 first) noexcept;

    models::Instruction build_instruction(std::string op, std::string dst,
                                          std::string src) const noexcept;
    std::string fmt_decode_rm(u8 mod, u8 rm, bool w) noexcept;
    std::string fmt_register(bool is_wide, u8 reg) const noexcept;
};

} // namespace sim::decode
