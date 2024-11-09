#include "common.hpp"

#include "instructions.hpp"
#include "table.hpp"

#include <vector>

#define MASK(x) x
#define EQUALS(x) x

namespace sim::decode::table {

namespace {
    constexpr auto NONE = BitField{0x00, 0};
    constexpr auto D(u8 x) { return BitField::create(x); }
    constexpr auto S(u8 x) { return BitField::create(x); }
    constexpr auto W(u8 x) { return BitField::create(x); }
    constexpr auto MOD(u8 x) { return BitField::create(x); }
    constexpr auto REG(u8 x) { return BitField::create(x); }
    constexpr auto RM(u8 x) { return BitField::create(x); }
} // namespace

// clang-format off
const std::vector<Encoding> instruction_encodings = {
   // NOTE(louis): see line ~90 of decoder.cpp on duplicate entries around imm to r/m arithmetic

   {instructions::Mnemonic::MOV,    MASK(0xFC), EQUALS(0b10001000), D(0x02), NONE,    W(0x01), MOD(0xC0), REG(0x38), RM(0x07), Encoding::RM_WITH_REG},
   {instructions::Mnemonic::MOV,    MASK(0xFE), EQUALS(0b11000110), NONE,    NONE,    W(0x01), MOD(0xC0), NONE,      RM(0x07), Encoding::IMM_TO_RM},
   {instructions::Mnemonic::MOV,    MASK(0xF0), EQUALS(0b10110000), NONE,    NONE,    W(0x08), NONE,      REG(0x07), NONE,     Encoding::IMM_TO_REG},

   {instructions::Mnemonic::ADD,    MASK(0xFC), EQUALS(0b00000000), D(0x02), NONE,    W(0x01), MOD(0xC0), REG(0x38), RM(0x07), Encoding::RM_WITH_REG},
   {instructions::Mnemonic::ADD,    MASK(0xFC), EQUALS(0b10000000), NONE,    S(0x02), W(0x01), MOD(0xC0), REG(0x38), RM(0x07), Encoding::IMM_TO_RM},
   {instructions::Mnemonic::ADD,    MASK(0xFE), EQUALS(0b00000100), NONE,    NONE,    W(0x01), NONE,      NONE,      NONE,     Encoding::IMM_TO_ACC},

   {instructions::Mnemonic::SUB,    MASK(0xFC), EQUALS(0b00101000), D(0x02), NONE,    W(0x01), MOD(0xC0), REG(0x38), RM(0x07), Encoding::RM_WITH_REG},
   {instructions::Mnemonic::SUB,    MASK(0xFC), EQUALS(0b10000000), NONE,    S(0x02), W(0x01), MOD(0xC0), REG(0x38), RM(0x07), Encoding::IMM_TO_RM},
   {instructions::Mnemonic::SUB,    MASK(0xFE), EQUALS(0b00101100), NONE,    NONE,    W(0x01), NONE,      NONE,      NONE,     Encoding::IMM_TO_ACC},

   {instructions::Mnemonic::CMP,    MASK(0xFC), EQUALS(0b00111000), D(0x02), NONE,    W(0x01), MOD(0xC0), REG(0x38), RM(0x07), Encoding::RM_WITH_REG},
   {instructions::Mnemonic::CMP,    MASK(0xFC), EQUALS(0b10000000), NONE,    S(0x02), W(0x01), MOD(0xC0), REG(0x38), RM(0x07), Encoding::IMM_TO_RM},
   {instructions::Mnemonic::CMP,    MASK(0xFE), EQUALS(0b00111100), NONE,    NONE,    W(0x01), NONE,      NONE,      NONE,     Encoding::IMM_TO_ACC},

   {instructions::Mnemonic::JE,     MASK(0xFF), EQUALS(0x74),       NONE,    NONE,    NONE,    NONE,      NONE,      NONE,     Encoding::JUMP},
   {instructions::Mnemonic::JL,     MASK(0xFF), EQUALS(0x7C),       NONE,    NONE,    NONE,    NONE,      NONE,      NONE,     Encoding::JUMP},
   {instructions::Mnemonic::JLE,    MASK(0xFF), EQUALS(0x7E),       NONE,    NONE,    NONE,    NONE,      NONE,      NONE,     Encoding::JUMP},
   {instructions::Mnemonic::JB,     MASK(0xFF), EQUALS(0x72),       NONE,    NONE,    NONE,    NONE,      NONE,      NONE,     Encoding::JUMP},
   {instructions::Mnemonic::JBE,    MASK(0xFF), EQUALS(0x76),       NONE,    NONE,    NONE,    NONE,      NONE,      NONE,     Encoding::JUMP},
   {instructions::Mnemonic::JP,     MASK(0xFF), EQUALS(0x7A),       NONE,    NONE,    NONE,    NONE,      NONE,      NONE,     Encoding::JUMP},
   {instructions::Mnemonic::JO,     MASK(0xFF), EQUALS(0x70),       NONE,    NONE,    NONE,    NONE,      NONE,      NONE,     Encoding::JUMP},
   {instructions::Mnemonic::JS,     MASK(0xFF), EQUALS(0x78),       NONE,    NONE,    NONE,    NONE,      NONE,      NONE,     Encoding::JUMP},
   {instructions::Mnemonic::JNE,    MASK(0xFF), EQUALS(0x75),       NONE,    NONE,    NONE,    NONE,      NONE,      NONE,     Encoding::JUMP},
   {instructions::Mnemonic::JNL,    MASK(0xFF), EQUALS(0x7D),       NONE,    NONE,    NONE,    NONE,      NONE,      NONE,     Encoding::JUMP},
   {instructions::Mnemonic::JG,     MASK(0xFF), EQUALS(0x7F),       NONE,    NONE,    NONE,    NONE,      NONE,      NONE,     Encoding::JUMP},
   {instructions::Mnemonic::JNB,    MASK(0xFF), EQUALS(0x73),       NONE,    NONE,    NONE,    NONE,      NONE,      NONE,     Encoding::JUMP},
   {instructions::Mnemonic::JA,     MASK(0xFF), EQUALS(0x77),       NONE,    NONE,    NONE,    NONE,      NONE,      NONE,     Encoding::JUMP},
   {instructions::Mnemonic::JNP,    MASK(0xFF), EQUALS(0x7B),       NONE,    NONE,    NONE,    NONE,      NONE,      NONE,     Encoding::JUMP},
   {instructions::Mnemonic::JNO,    MASK(0xFF), EQUALS(0x71),       NONE,    NONE,    NONE,    NONE,      NONE,      NONE,     Encoding::JUMP},
   {instructions::Mnemonic::JNS,    MASK(0xFF), EQUALS(0x79),       NONE,    NONE,    NONE,    NONE,      NONE,      NONE,     Encoding::JUMP},
   {instructions::Mnemonic::LOOP,   MASK(0xFF), EQUALS(0xE2),       NONE,    NONE,    NONE,    NONE,      NONE,      NONE,     Encoding::JUMP},
   {instructions::Mnemonic::LOOPZ,  MASK(0xFF), EQUALS(0xE1),       NONE,    NONE,    NONE,    NONE,      NONE,      NONE,     Encoding::JUMP},
   {instructions::Mnemonic::LOOPNZ, MASK(0xFF), EQUALS(0xE0),       NONE,    NONE,    NONE,    NONE,      NONE,      NONE,     Encoding::JUMP},
   {instructions::Mnemonic::JCXZ,   MASK(0xFF), EQUALS(0xE3),       NONE,    NONE,    NONE,    NONE,      NONE,      NONE,     Encoding::JUMP}
};
// clang-format on

} // namespace sim::decode::table
