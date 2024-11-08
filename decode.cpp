#include "common.hpp"

#include "decode.hpp"
#include "instructions.hpp"
#include "memory.hpp"
#include "table.hpp"

#include <array>
#include <optional>
#include <vector>

namespace sim::decode {
namespace {
    static constexpr std::array<std::pair<u8, u8>, 8> EFFECTIVE_ADDRESSES = {{
        {instructions::registers::BX, instructions::registers::SI},
        {instructions::registers::BX, instructions::registers::DI},
        {instructions::registers::BP, instructions::registers::SI},
        {instructions::registers::BP, instructions::registers::DI},
        {instructions::registers::SI, instructions::registers::NONE},
        {instructions::registers::DI, instructions::registers::NONE},
        {instructions::registers::BP, instructions::registers::NONE},
        {instructions::registers::BX, instructions::registers::NONE},
    }};
}

std::optional<instructions::Instruction> Decoder::try_decode(const std::vector<u8> &memory,
                                                             u8 address) noexcept {
    sim::mem::MemoryReader reader(memory, address);
    u8 byte = reader.byte();

    for (const auto &encoding : table::instruction_encodings) {
        if ((byte & encoding.mask) != encoding.equals) {
            continue;
        }

        instructions::Instruction instruction;
        switch (encoding.type) {
        case table::Encoding::Type::RM_WITH_REG:
            instruction = rm_with_reg(reader, encoding, byte);
            break;
        case table::Encoding::Type::IMM_TO_RM:
            instruction = imm_to_rm(reader, encoding, byte);
            break;
        case table::Encoding::Type::IMM_TO_REG:
            instruction = imm_to_reg(reader, encoding, byte);
            break;
        case table::Encoding::Type::IMM_TO_ACC:
            instruction = imm_to_acc(reader, encoding, byte);
            break;
        case table::Encoding::Type::JUMP:
            instruction = jump(reader, encoding);
            break;
        }

        return instruction;
    }

    return std::nullopt;
}

instructions::Instruction Decoder::rm_with_reg(sim::mem::MemoryReader &reader,
                                               const table::Encoding &encoding, u8 first) noexcept {
    auto fields = instructions::InstructionFields::from(encoding, first, reader.byte());

    instructions::Operand reg = instructions::Operand::from_register(fields.reg, fields.is_wide);
    instructions::Operand rm = decode_rm(reader, fields.is_wide, fields.mod, fields.rm);

    return instructions::Instruction{
        .mnemonic = std::string(encoding.mnemonic),
        .dst = fields.is_reg_dst ? reg : rm,
        .src = fields.is_reg_dst ? rm : reg,
        .address = reader.get_start_address(),
        .bytes = reader.get_bytes_read(),
    };
}

instructions::Instruction Decoder::imm_to_rm(sim::mem::MemoryReader &reader,
                                             const table::Encoding &encoding, u8 first) noexcept {
    auto fields = instructions::InstructionFields::from(encoding, first, reader.byte());

    instructions::Operand rm = decode_rm(reader, fields.is_wide, fields.mod, fields.rm);

    u16 imm;
    if (fields.is_wide && (encoding.mnemonic == "mov" || !fields.is_sign_extended)) {
        imm = reader.word();
    } else {
        imm = reader.byte();
        if (fields.is_sign_extended && (imm & 0x80)) {
            imm |= 0xFF00;
        }
    }

    // TODO(louis): this is bad, many identical entries in the table for different encodings.
    // maybe we could abstract mask/equals into like a comparison object and we can have 1or2
    // comparisons to be true before an encoding matches
    std::string mnemonic;
    switch (fields.reg) {
    case 0b000:
        mnemonic = "add";
        break;
    case 0b101:
        mnemonic = "sub";
        break;
    case 0b111:
        mnemonic = "cmp";
        break;
    }

    return instructions::Instruction{
        .mnemonic = mnemonic,
        .dst = rm,
        .src = instructions::Operand::from_immediate(imm),
        .address = reader.get_start_address(),
        .bytes = reader.get_bytes_read(),
    };
}

instructions::Instruction Decoder::imm_to_reg(sim::mem::MemoryReader &reader,
                                              const table::Encoding &encoding, u8 first) noexcept {
    auto fields = instructions::InstructionFields::from(encoding, first);

    instructions::Operand reg = instructions::Operand::from_register(fields.reg, fields.is_wide);
    instructions::Operand imm =
        instructions::Operand::from_immediate(fields.is_wide ? reader.word() : reader.byte());

    return instructions::Instruction{
        .mnemonic = std::string(encoding.mnemonic),
        .dst = reg,
        .src = imm,
        .address = reader.get_start_address(),
        .bytes = reader.get_bytes_read(),
    };
}

instructions::Instruction Decoder::imm_to_acc(sim::mem::MemoryReader &reader,
                                              const table::Encoding &encoding, u8 first) noexcept {
    auto fields = instructions::InstructionFields::from(encoding, first);

    instructions::Operand reg = instructions::Operand::from_register(0b000, fields.is_wide);
    instructions::Operand imm =
        instructions::Operand::from_immediate(fields.is_wide ? reader.word() : reader.byte());

    return instructions::Instruction{
        .mnemonic = std::string(encoding.mnemonic),
        .dst = reg,
        .src = imm,
        .address = reader.get_start_address(),
        .bytes = reader.get_bytes_read(),
    };
}
instructions::Instruction Decoder::jump(sim::mem::MemoryReader &reader,
                                        const table::Encoding &encoding) noexcept {
    instructions::Operand imm = instructions::Operand::from_immediate(reader.byte());

    return instructions::Instruction{
        .mnemonic = std::string(encoding.mnemonic),
        .dst = imm,
        .src = instructions::Operand::none(),
        .address = reader.get_start_address(),
        .bytes = reader.get_bytes_read(),
    };
}

instructions::Operand Decoder::decode_rm(sim::mem::MemoryReader &reader, bool is_wide, u8 mod,
                                         u8 rm) noexcept {
    using instructions::Operand;

    switch (mod) {
    case 0b00: {
        if (rm == 0b110) {
            return Operand::direct_address(reader.word(), is_wide);
        }

        auto [reg1, reg2] = EFFECTIVE_ADDRESSES[rm];
        return Operand::effective_address(reg1, reg2, 0, is_wide);
    }

    case 0b01: {
        auto [reg1, reg2] = EFFECTIVE_ADDRESSES[rm];
        return Operand::effective_address(reg1, reg2, reader.byte(), is_wide);
    }

    case 0b10: {
        auto [reg1, reg2] = EFFECTIVE_ADDRESSES[rm];
        return Operand::effective_address(reg1, reg2, reader.word(), is_wide);
    }

    case 0b11:
        return Operand::from_register(rm, is_wide);

    default:
        UNREACHABLE();
    }
}

} // namespace sim::decode
