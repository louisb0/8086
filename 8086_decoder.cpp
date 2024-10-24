#include "8086_decoder.hpp"
#include "8086_matcher.hpp"

#include <array>
#include <cassert>
#include <optional>

static constexpr std::array<std::string_view, 8> WORD_REGISTERS = {
    "ax", "cx", "dx", "bx", "sp", "bp", "si", "di"};

static constexpr std::array<std::string_view, 8> BYTE_REGISTERS = {
    "al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"};

static constexpr std::array<std::string_view, 8> EFFECTIVE_ADDRESS = {
    "bx + si", "bx + di", "bp + si", "bp + di", "si", "di", "bp", "bx"};

std::optional<Instruction>
InstructionDecoder::try_decode(const InstructionPattern &pattern, u8 byte) noexcept {
    if (!InstructionMatcher::matches(pattern, byte)) {
        return std::nullopt;
    }

    // TODO(louis): Necessary?
    assert(bytes.size() == 0);

    bytes.push_back(byte);

    Instruction instruction;

    switch (pattern.type) {
    case InstructionPattern::RM_WITH_REG:
        instruction = rm_with_reg(pattern, byte);
        break;
    case InstructionPattern::IMM_TO_RM:
        instruction = imm_to_rm(pattern, byte);
        break;
    case InstructionPattern::IMM_TO_REG:
        instruction = imm_to_reg(pattern, byte);
        break;
    case InstructionPattern::IMM_TO_ACC:
        instruction = imm_to_acc(pattern, byte);
        break;
    case InstructionPattern::JUMP:
        instruction = jump(pattern, byte);
        break;
    }

    current_address += instruction.bytes.size();
    bytes.clear();

    return instruction;
}

Instruction InstructionDecoder::rm_with_reg(const InstructionPattern &pattern, u8 first) {
    bool d = InstructionMatcher::read_field(pattern.d, first);
    bool w = InstructionMatcher::read_field(pattern.w, first);

    u8 second = read_byte();

    u8 mod = InstructionMatcher::read_field(pattern.mod, second);
    u8 reg = InstructionMatcher::read_field(pattern.reg, second);
    u8 rm = InstructionMatcher::read_field(pattern.rm, second);

    std::string reg_str = fmt_register(w, reg);
    std::string rm_str = fmt_decode_rm(mod, rm, w);

    return build_instruction(std::string(pattern.op), d ? reg_str : rm_str,
                             d ? rm_str : reg_str);
}

Instruction InstructionDecoder::imm_to_rm(const InstructionPattern &pattern, u8 first) {
    bool s = InstructionMatcher::read_field(pattern.s, first);
    bool w = InstructionMatcher::read_field(pattern.w, first);

    u8 second = read_byte();

    u8 mod = InstructionMatcher::read_field(pattern.mod, second);
    u8 rm = InstructionMatcher::read_field(pattern.rm, second);

    std::string rm_str = fmt_decode_rm(mod, rm, w);

    u16 imm;
    if (w && (pattern.op == "mov" || !s)) {
        imm = read_word();
    } else {
        imm = read_byte();

        if (s && (imm & 0x80)) {
            imm |= 0xFF00;
        }
    }

    return build_instruction(std::string(pattern.op), rm_str, std::to_string(imm));
}

// NOTE(louis): Triggered by mov only.
Instruction InstructionDecoder::imm_to_reg(const InstructionPattern &pattern, u8 first) {
    bool w = InstructionMatcher::read_field(pattern.w, first);
    u8 reg = InstructionMatcher::read_field(pattern.reg, first);

    u16 imm = w ? read_word() : read_byte();

    return build_instruction(std::string(pattern.op), fmt_register(w, reg),
                             std::to_string(imm));
}

Instruction InstructionDecoder::imm_to_acc(const InstructionPattern &pattern, u8 first) {
    bool w = InstructionMatcher::read_field(pattern.w, first);

    u16 imm = w ? read_word() : read_byte();

    return build_instruction(std::string(pattern.op), w ? "ax" : "al",
                             std::to_string(imm));
}

Instruction InstructionDecoder::jump(const InstructionPattern &pattern, u8 first) {
    u8 second = read_byte();

    // TODO(louis): static_cast?
    return build_instruction(std::string(pattern.op),
                             std::to_string(static_cast<int>(second)), "");
}

// TODO(louis): Move semantics, copying, conversions from string_view in pattern table.
Instruction InstructionDecoder::build_instruction(std::string op, std::string dst,
                                                  std::string src) {
    return Instruction{std::string(op), std::string(dst), std::string(src),
                       current_address, std::move(bytes)};
}

std::string InstructionDecoder::fmt_register(bool is_wide, u8 reg) {
    return std::string(is_wide ? WORD_REGISTERS[reg] : BYTE_REGISTERS[reg]);
}

std::string InstructionDecoder::fmt_decode_rm(u8 mod, u8 rm, bool w) {
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
        return "[" + std::string(EFFECTIVE_ADDRESS[rm]) + " + " + std::to_string(disp) +
               "]";
    }

    case 0b10: {
        u16 disp = read_word();
        if (disp == 0)
            return "[" + std::string(EFFECTIVE_ADDRESS[rm]) + "]";
        return "[" + std::string(EFFECTIVE_ADDRESS[rm]) + " + " + std::to_string(disp) +
               "]";
    }

    case 0b11:
        return fmt_register(w, rm);

    default:
        assert(!"Unreachable.");
        return "";
    }
}
