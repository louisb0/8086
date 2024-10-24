#include "8086_matcher.hpp"

#include <vector>

// TODO(louis): Reduce duplication, align the table.

#define MASK(x) x
#define EQUALS(x) x

#define NONE() {0x00}
#define D(x) {x}
#define S(x) {x}
#define W(x) {x}
#define MOD(x) {x}
#define REG(x) {x}
#define RM(x) {x}

// clang-format off
const std::vector<InstructionPattern> InstructionMatcher::patterns = {
    {"mov",    MASK(0xFC), EQUALS(0b10001000),  D(0x02), NONE(), W(0x01), MOD(0xC0), REG(0x38), RM(0x07), InstructionPattern::RM_WITH_REG},
    {"mov",    MASK(0xFE), EQUALS(0b11000110), NONE(),  NONE(), W(0x01), MOD(0xC0), NONE(),    RM(0x07), InstructionPattern::IMM_TO_RM},
    {"mov",    MASK(0xF0), EQUALS(0b10110000),    NONE(),  NONE(), W(0x08), NONE(),    REG(0x07), NONE(),   InstructionPattern::IMM_TO_REG},

    {"add",    MASK(0xFC), EQUALS(0b00000000),  D(0x02), NONE(), W(0x01), MOD(0xC0), REG(0x38), RM(0x07), InstructionPattern::RM_WITH_REG},
    {"add",    MASK(0xFC), EQUALS(0b10000000),  NONE(),  S(0x02), W(0x01), MOD(0xC0), NONE(),    RM(0x07), InstructionPattern::IMM_TO_RM},
    {"add",    MASK(0xFE), EQUALS(0b00000100), NONE(),  NONE(), W(0x01), NONE(),    NONE(),    NONE(),   InstructionPattern::IMM_TO_ACC},

    {"sub",    MASK(0xFC), EQUALS(0b00101000),  D(0x02), NONE(), W(0x01), MOD(0xC0), REG(0x38), RM(0x07), InstructionPattern::RM_WITH_REG},
    {"sub",    MASK(0xFC), EQUALS(0b10000000),  NONE(),  S(0x02), W(0x01), MOD(0xC0), NONE(),    RM(0x07), InstructionPattern::IMM_TO_RM},
    {"sub",    MASK(0xFE), EQUALS(0b00101100), NONE(),  NONE(), W(0x01), NONE(),    NONE(),    NONE(),   InstructionPattern::IMM_TO_ACC},

    {"cmp",    MASK(0xFC), EQUALS(0b00111000),  D(0x02), NONE(), W(0x01), MOD(0xC0), REG(0x38), RM(0x07), InstructionPattern::RM_WITH_REG},
    {"cmp",    MASK(0xFC), EQUALS(0b10000000),  NONE(),  S(0x02), W(0x01), MOD(0xC0), NONE(),    RM(0x07), InstructionPattern::IMM_TO_RM},
    {"cmp",    MASK(0xFE), EQUALS(0b00111100), NONE(),  NONE(), W(0x01), NONE(),    NONE(),    NONE(),   InstructionPattern::IMM_TO_ACC},

    {"je",     MASK(0xFF), EQUALS(0x74),      NONE(),  NONE(), NONE(),  NONE(),    NONE(),    NONE(),   InstructionPattern::JUMP},
    {"jl",     MASK(0xFF), EQUALS(0x7C),      NONE(),  NONE(), NONE(),  NONE(),    NONE(),    NONE(),   InstructionPattern::JUMP},
    {"jle",    MASK(0xFF), EQUALS(0x7E),      NONE(),  NONE(), NONE(),  NONE(),    NONE(),    NONE(),   InstructionPattern::JUMP},
    {"jb",     MASK(0xFF), EQUALS(0x72),      NONE(),  NONE(), NONE(),  NONE(),    NONE(),    NONE(),   InstructionPattern::JUMP},
    {"jbe",    MASK(0xFF), EQUALS(0x76),      NONE(),  NONE(), NONE(),  NONE(),    NONE(),    NONE(),   InstructionPattern::JUMP},
    {"jp",     MASK(0xFF), EQUALS(0x7A),      NONE(),  NONE(), NONE(),  NONE(),    NONE(),    NONE(),   InstructionPattern::JUMP},
    {"jo",     MASK(0xFF), EQUALS(0x70),      NONE(),  NONE(), NONE(),  NONE(),    NONE(),    NONE(),   InstructionPattern::JUMP},
    {"js",     MASK(0xFF), EQUALS(0x78),      NONE(),  NONE(), NONE(),  NONE(),    NONE(),    NONE(),   InstructionPattern::JUMP},
    {"jne",    MASK(0xFF), EQUALS(0x75),      NONE(),  NONE(), NONE(),  NONE(),    NONE(),    NONE(),   InstructionPattern::JUMP},
    {"jnl",    MASK(0xFF), EQUALS(0x7D),      NONE(),  NONE(), NONE(),  NONE(),    NONE(),    NONE(),   InstructionPattern::JUMP},
    {"jg",     MASK(0xFF), EQUALS(0x7F),      NONE(),  NONE(), NONE(),  NONE(),    NONE(),    NONE(),   InstructionPattern::JUMP},
    {"jnb",    MASK(0xFF), EQUALS(0x73),      NONE(),  NONE(), NONE(),  NONE(),    NONE(),    NONE(),   InstructionPattern::JUMP},
    {"ja",     MASK(0xFF), EQUALS(0x77),      NONE(),  NONE(), NONE(),  NONE(),    NONE(),    NONE(),   InstructionPattern::JUMP},
    {"jnp",    MASK(0xFF), EQUALS(0x7B),      NONE(),  NONE(), NONE(),  NONE(),    NONE(),    NONE(),   InstructionPattern::JUMP},
    {"jno",    MASK(0xFF), EQUALS(0x71),      NONE(),  NONE(), NONE(),  NONE(),    NONE(),    NONE(),   InstructionPattern::JUMP},
    {"jns",    MASK(0xFF), EQUALS(0x79),      NONE(),  NONE(), NONE(),  NONE(),    NONE(),    NONE(),   InstructionPattern::JUMP},
    {"loop",   MASK(0xFF), EQUALS(0xE2),      NONE(),  NONE(), NONE(),  NONE(),    NONE(),    NONE(),   InstructionPattern::JUMP},
    {"loopz",  MASK(0xFF), EQUALS(0xE1),      NONE(),  NONE(), NONE(),  NONE(),    NONE(),    NONE(),   InstructionPattern::JUMP},
    {"loopnz", MASK(0xFF), EQUALS(0xE0),      NONE(),  NONE(), NONE(),  NONE(),    NONE(),    NONE(),   InstructionPattern::JUMP},
    {"jcxz",   MASK(0xFF), EQUALS(0xE3),      NONE(),  NONE(), NONE(),  NONE(),    NONE(),    NONE(),   InstructionPattern::JUMP}
};
// clang-format on
