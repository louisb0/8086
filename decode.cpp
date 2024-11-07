#include "common.hpp"

#include "decode.hpp"
#include "instructions.hpp"
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

std::optional<instructions::Instruction> Decoder::try_decode(const table::Encoding &encoding,
                                                             u8 byte) noexcept {
    if ((byte & encoding.mask) != encoding.equals) {
        return std::nullopt;
    }

    // TODO(louis): this is hacky. we need this for the very first byte read *externally*.
    // this is because do not own the input stream (we should). it is owned by the callee.
    // this should be fixed by creating a memory abstraction, not using a file.
    bytes.push_back(byte);

    instructions::Instruction instruction;

    switch (encoding.type) {
    case table::Encoding::Type::RM_WITH_REG:
        instruction = rm_with_reg(encoding, byte);
        break;
    case table::Encoding::Type::IMM_TO_RM:
        instruction = imm_to_rm(encoding, byte);
        break;
    case table::Encoding::Type::IMM_TO_REG:
        instruction = imm_to_reg(encoding, byte);
        break;
    case table::Encoding::Type::IMM_TO_ACC:
        instruction = imm_to_acc(encoding, byte);
        break;
    case table::Encoding::Type::JUMP:
        instruction = jump(encoding);
        break;
    }

    current_address += instruction.bytes.size();
    bytes.clear();

    return instruction;
}

instructions::Instruction Decoder::rm_with_reg(const table::Encoding &encoding, u8 first) noexcept {
    auto fields = InstructionFields::from(encoding, first, read_byte());

    instructions::Operand reg = instructions::Operand::from_register(fields.reg, fields.is_wide);
    instructions::Operand rm = decode_rm(fields.is_wide, fields.mod, fields.rm);

    return instructions::Instruction{
        .mnemonic = std::string(encoding.mnemonic),
        .dst = fields.is_reg_dst ? reg : rm,
        .src = fields.is_reg_dst ? rm : reg,
        .address = current_address,
        .bytes = bytes,
    };
}
instructions::Instruction Decoder::imm_to_rm(const table::Encoding &encoding, u8 first) noexcept {
    auto fields = InstructionFields::from(encoding, first, read_byte());

    instructions::Operand rm = decode_rm(fields.is_wide, fields.mod, fields.rm);

    u16 imm;
    if (fields.is_wide && (encoding.mnemonic == "mov" || !fields.is_sign_extended)) {
        imm = read_word();
    } else {
        imm = read_byte();
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
        .address = current_address,
        .bytes = bytes,
    };
}

// NOTE(louis): mov only
instructions::Instruction Decoder::imm_to_reg(const table::Encoding &encoding, u8 first) noexcept {
    auto fields = InstructionFields::from(encoding, first);

    instructions::Operand reg = instructions::Operand::from_register(fields.reg, fields.is_wide);
    instructions::Operand imm =
        instructions::Operand::from_immediate(fields.is_wide ? read_word() : read_byte());

    return instructions::Instruction{
        .mnemonic = std::string(encoding.mnemonic),
        .dst = reg,
        .src = imm,
        .address = current_address,
        .bytes = bytes,
    };
}

instructions::Instruction Decoder::imm_to_acc(const table::Encoding &encoding, u8 first) noexcept {
    auto fields = InstructionFields::from(encoding, first);

    instructions::Operand reg = instructions::Operand::from_register(0b000, fields.is_wide);
    instructions::Operand imm =
        instructions::Operand::from_immediate(fields.is_wide ? read_word() : read_byte());

    return instructions::Instruction{
        .mnemonic = std::string(encoding.mnemonic),
        .dst = reg,
        .src = imm,
        .address = current_address,
        .bytes = bytes,
    };
}

instructions::Instruction Decoder::jump(const table::Encoding &encoding) noexcept {
    instructions::Operand imm = instructions::Operand::from_immediate(read_byte());

    return instructions::Instruction{
        .mnemonic = std::string(encoding.mnemonic),
        .dst = imm,
        .src = instructions::Operand::none(),
        .address = current_address,
        .bytes = bytes,
    };
}

instructions::Operand Decoder::decode_rm(bool is_wide, u8 mod, u8 rm) noexcept {
    using instructions::Operand;

    switch (mod) {
    case 0b00: {
        if (rm == 0b110) {
            return Operand::direct_address(read_word(), is_wide);
        }

        auto [reg1, reg2] = EFFECTIVE_ADDRESSES[rm];
        return Operand::effective_address(reg1, reg2, 0, is_wide);
    }

    case 0b01: {
        auto [reg1, reg2] = EFFECTIVE_ADDRESSES[rm];
        return Operand::effective_address(reg1, reg2, read_byte(), is_wide);
    }

    case 0b10: {
        auto [reg1, reg2] = EFFECTIVE_ADDRESSES[rm];
        return Operand::effective_address(reg1, reg2, read_word(), is_wide);
    }

    case 0b11:
        return Operand::from_register(rm, is_wide);

    default:
        UNREACHABLE();
    }
}

} // namespace sim::decode
