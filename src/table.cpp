#include "common.hpp"

#include "instructions.hpp"
#include "table.hpp"

#include <vector>

namespace sim::decode::table {

namespace {
    constexpr auto NONE = BitField{0x00, 0};
    constexpr auto D(u8 x) { return BitField::create(x); }
    constexpr auto S(u8 x) { return BitField::create(x); }
    constexpr auto W(u8 x) { return BitField::create(x); }
    constexpr auto MOD(u8 x) { return BitField::create(x); }
    constexpr auto REG(u8 x) { return BitField::create(x); }
    constexpr auto RM(u8 x) { return BitField::create(x); }

    constexpr auto FIRST(u8 mask, u8 equals) { return MatchCondition::create(mask, equals); }
    constexpr auto SECOND(u8 mask, u8 equals) { return MatchCondition::create(mask, equals); }
    constexpr auto NO_MATCH = MatchCondition::none();
} // namespace

// clang-format off
const std::vector<Encoding> instruction_encodings = {
    {instructions::Mnemonic::MOV,    FIRST(0xFC, 0b100010),  NO_MATCH,            D(0x02), NONE,    W(0x01), MOD(0xC0), REG(0x38), RM(0x07), Encoding::RM_WITH_REG},
    {instructions::Mnemonic::MOV,    FIRST(0xFE, 0b1100011), NO_MATCH,            NONE,    NONE,    W(0x01), MOD(0xC0), NONE,      RM(0x07), Encoding::IMM_WITH_RM},
    {instructions::Mnemonic::MOV,    FIRST(0xF0, 0b1011),    NO_MATCH,            NONE,    NONE,    W(0x08), NONE,      REG(0x07), NONE,     Encoding::IMM_TO_REG},

    {instructions::Mnemonic::ADD,    FIRST(0xFC, 0b000000),  NO_MATCH,            D(0x02), NONE,    W(0x01), MOD(0xC0), REG(0x38), RM(0x07), Encoding::RM_WITH_REG},
    {instructions::Mnemonic::ADD,    FIRST(0xFC, 0b100000),  SECOND(0x38, 0b000), NONE,    S(0x02), W(0x01), MOD(0xC0), REG(0x38), RM(0x07), Encoding::IMM_WITH_RM},
    {instructions::Mnemonic::ADD,    FIRST(0xFE, 0b0000010), NO_MATCH,            NONE,    NONE,    W(0x01), NONE,      NONE,      NONE,     Encoding::IMM_WITH_ACC},

    {instructions::Mnemonic::SUB,    FIRST(0xFC, 0b001010),  NO_MATCH,            D(0x02), NONE,    W(0x01), MOD(0xC0), REG(0x38), RM(0x07), Encoding::RM_WITH_REG},
    {instructions::Mnemonic::SUB,    FIRST(0xFC, 0b100000),  SECOND(0x38, 0b101), NONE,    S(0x02), W(0x01), MOD(0xC0), REG(0x38), RM(0x07), Encoding::IMM_WITH_RM},
    {instructions::Mnemonic::SUB,    FIRST(0xFE, 0b0010110), NO_MATCH,            NONE,    NONE,    W(0x01), NONE,      NONE,      NONE,     Encoding::IMM_WITH_ACC},

    {instructions::Mnemonic::CMP,    FIRST(0xFC, 0b001110),  NO_MATCH,            D(0x02), NONE,    W(0x01), MOD(0xC0), REG(0x38), RM(0x07), Encoding::RM_WITH_REG},
    {instructions::Mnemonic::CMP,    FIRST(0xFC, 0b100000),  SECOND(0x38, 0b111), NONE,    S(0x02), W(0x01), MOD(0xC0), REG(0x38), RM(0x07), Encoding::IMM_WITH_RM},
    {instructions::Mnemonic::CMP,    FIRST(0xFE, 0b0011110), NO_MATCH,            NONE,    NONE,    W(0x01), NONE,      NONE,      NONE,     Encoding::IMM_WITH_ACC},

    {instructions::Mnemonic::JE,     FIRST(0xFF, 0x74),      NO_MATCH,            NONE,    NONE,    NONE,    NONE,      NONE,      NONE,     Encoding::JUMP},
    {instructions::Mnemonic::JL,     FIRST(0xFF, 0x7C),      NO_MATCH,            NONE,    NONE,    NONE,    NONE,      NONE,      NONE,     Encoding::JUMP},
    {instructions::Mnemonic::JLE,    FIRST(0xFF, 0x7E),      NO_MATCH,            NONE,    NONE,    NONE,    NONE,      NONE,      NONE,     Encoding::JUMP},
    {instructions::Mnemonic::JB,     FIRST(0xFF, 0x72),      NO_MATCH,            NONE,    NONE,    NONE,    NONE,      NONE,      NONE,     Encoding::JUMP},
    {instructions::Mnemonic::JBE,    FIRST(0xFF, 0x76),      NO_MATCH,            NONE,    NONE,    NONE,    NONE,      NONE,      NONE,     Encoding::JUMP},
    {instructions::Mnemonic::JP,     FIRST(0xFF, 0x7A),      NO_MATCH,            NONE,    NONE,    NONE,    NONE,      NONE,      NONE,     Encoding::JUMP},
    {instructions::Mnemonic::JO,     FIRST(0xFF, 0x70),      NO_MATCH,            NONE,    NONE,    NONE,    NONE,      NONE,      NONE,     Encoding::JUMP},
    {instructions::Mnemonic::JS,     FIRST(0xFF, 0x78),      NO_MATCH,            NONE,    NONE,    NONE,    NONE,      NONE,      NONE,     Encoding::JUMP},
    {instructions::Mnemonic::JNE,    FIRST(0xFF, 0x75),      NO_MATCH,            NONE,    NONE,    NONE,    NONE,      NONE,      NONE,     Encoding::JUMP},
    {instructions::Mnemonic::JNL,    FIRST(0xFF, 0x7D),      NO_MATCH,            NONE,    NONE,    NONE,    NONE,      NONE,      NONE,     Encoding::JUMP},
    {instructions::Mnemonic::JG,     FIRST(0xFF, 0x7F),      NO_MATCH,            NONE,    NONE,    NONE,    NONE,      NONE,      NONE,     Encoding::JUMP},
    {instructions::Mnemonic::JNB,    FIRST(0xFF, 0x73),      NO_MATCH,            NONE,    NONE,    NONE,    NONE,      NONE,      NONE,     Encoding::JUMP},
    {instructions::Mnemonic::JA,     FIRST(0xFF, 0x77),      NO_MATCH,            NONE,    NONE,    NONE,    NONE,      NONE,      NONE,     Encoding::JUMP},
    {instructions::Mnemonic::JNP,    FIRST(0xFF, 0x7B),      NO_MATCH,            NONE,    NONE,    NONE,    NONE,      NONE,      NONE,     Encoding::JUMP},
    {instructions::Mnemonic::JNO,    FIRST(0xFF, 0x71),      NO_MATCH,            NONE,    NONE,    NONE,    NONE,      NONE,      NONE,     Encoding::JUMP},
    {instructions::Mnemonic::JNS,    FIRST(0xFF, 0x79),      NO_MATCH,            NONE,    NONE,    NONE,    NONE,      NONE,      NONE,     Encoding::JUMP},
    {instructions::Mnemonic::LOOP,   FIRST(0xFF, 0xE2),      NO_MATCH,            NONE,    NONE,    NONE,    NONE,      NONE,      NONE,     Encoding::JUMP},
    {instructions::Mnemonic::LOOPZ,  FIRST(0xFF, 0xE1),      NO_MATCH,            NONE,    NONE,    NONE,    NONE,      NONE,      NONE,     Encoding::JUMP},
    {instructions::Mnemonic::LOOPNZ, FIRST(0xFF, 0xE0),      NO_MATCH,            NONE,    NONE,    NONE,    NONE,      NONE,      NONE,     Encoding::JUMP},
    {instructions::Mnemonic::JCXZ,   FIRST(0xFF, 0xE3),      NO_MATCH,            NONE,    NONE,    NONE,    NONE,      NONE,      NONE,     Encoding::JUMP}
};
// clang-format on

} // namespace sim::decode::table
