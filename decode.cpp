#include "common.hpp"

#include "decode.hpp"
#include "instructions.hpp"
#include "table.hpp"

#include <array>
#include <optional>
#include <string>
#include <vector>

namespace sim::decode {

namespace {
    constexpr std::array<std::string_view, 8> WORD_REGISTERS = {"ax", "cx", "dx", "bx",
                                                                "sp", "bp", "si", "di"};

    constexpr std::array<std::string_view, 8> BYTE_REGISTERS = {"al", "cl", "dl", "bl",
                                                                "ah", "ch", "dh", "bh"};

    constexpr std::array<std::string_view, 8> EFFECTIVE_ADDRESS = {
        "bx + si", "bx + di", "bp + si", "bp + di", "si", "di", "bp", "bx"};
} // namespace

std::optional<instructions::Instruction> Decoder::try_decode(const table::Encoding &encoding,
                                                             u8 byte) noexcept {
    if ((byte & encoding.mask) != encoding.equals) {
        return std::nullopt;
    }

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
        instruction = jump(encoding, byte);
        break;
    }

    current_address += instruction.bytes.size();
    bytes.clear();

    return instruction;
}

instructions::Instruction Decoder::rm_with_reg(const table::Encoding &encoding, u8 first) noexcept {
    bool d = encoding.d.read(first);
    bool w = encoding.w.read(first);

    u8 second = read_byte();

    u8 mod = encoding.mod.read(second);
    u8 reg = encoding.reg.read(second);
    u8 rm = encoding.rm.read(second);

    std::string reg_str = fmt_register(w, reg);
    std::string rm_str = fmt_decode_rm(mod, rm, w);

    return build_instruction(std::string(encoding.mnemonic), d ? reg_str : rm_str,
                             d ? rm_str : reg_str);
}

instructions::Instruction Decoder::imm_to_rm(const table::Encoding &encoding, u8 first) noexcept {
    bool s = encoding.s.read(first);
    bool w = encoding.w.read(first);

    u8 second = read_byte();

    u8 mod = encoding.mod.read(second);
    u8 rm = encoding.rm.read(second);

    std::string rm_str = fmt_decode_rm(mod, rm, w);

    u16 imm;
    if (w && (encoding.mnemonic == "mov" || !s)) {
        imm = read_word();
    } else {
        imm = read_byte();
        if (s && (imm & 0x80)) {
            imm |= 0xFF00;
        }
    }

    return build_instruction(std::string(encoding.mnemonic), rm_str, std::to_string(imm));
}

instructions::Instruction Decoder::imm_to_reg(const table::Encoding &encoding, u8 first) noexcept {
    bool w = encoding.w.read(first);
    u8 reg = encoding.reg.read(first);

    u16 imm = w ? read_word() : read_byte();
    std::string reg_str = fmt_register(w, reg);

    return build_instruction(std::string(encoding.mnemonic), reg_str, std::to_string(imm));
}

instructions::Instruction Decoder::imm_to_acc(const table::Encoding &encoding, u8 first) noexcept {
    bool w = encoding.w.read(first);
    u16 imm = w ? read_word() : read_byte();

    return build_instruction(std::string(encoding.mnemonic), w ? "ax" : "al", std::to_string(imm));
}

instructions::Instruction Decoder::jump(const table::Encoding &encoding, u8 first) noexcept {
    u8 second = read_byte();
    return build_instruction(std::string(encoding.mnemonic), std::to_string(second), "");
}

instructions::Instruction Decoder::build_instruction(std::string op, std::string dst,
                                                     std::string src) const noexcept {
    return instructions::Instruction{std::move(op), std::move(dst), std::move(src), current_address,
                                     bytes};
}

std::string Decoder::fmt_register(bool is_wide, u8 reg) const noexcept {
    return std::string(is_wide ? WORD_REGISTERS[reg] : BYTE_REGISTERS[reg]);
}

std::string Decoder::fmt_decode_rm(u8 mod, u8 rm, bool w) noexcept {
    switch (mod) {
    case 0b00:
        if (rm == 0b110) {
            u16 direct = read_word();
            return "[" + std::to_string(direct) + "]";
        }
        return "[" + std::string(EFFECTIVE_ADDRESS[rm]) + "]";

    case 0b01: {
        u8 disp = read_byte();
        if (disp == 0)
            return "[" + std::string(EFFECTIVE_ADDRESS[rm]) + "]";
        return "[" + std::string(EFFECTIVE_ADDRESS[rm]) + " + " + std::to_string(disp) + "]";
    }

    case 0b10: {
        u16 disp = read_word();
        if (disp == 0)
            return "[" + std::string(EFFECTIVE_ADDRESS[rm]) + "]";
        return "[" + std::string(EFFECTIVE_ADDRESS[rm]) + " + " + std::to_string(disp) + "]";
    }

    case 0b11:
        return fmt_register(w, rm);

    default:
        assert(!"Unreachable.");
        return "";
    }
}

} // namespace sim::decode
