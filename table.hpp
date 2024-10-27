#pragma once

#include "common.hpp"

#include "models.hpp"

#include <string_view>
#include <vector>

namespace sim::decode::table {

struct Encoding {
    std::string_view mnemonic;

    // NOTE(louis): Replace with models::BitField?
    u8 mask;
    u8 equals;

    models::BitField d;
    models::BitField s;
    models::BitField w;
    models::BitField mod;
    models::BitField reg;
    models::BitField rm;

    enum Type {
        RM_WITH_REG,
        IMM_TO_RM,
        IMM_TO_ACC,
        IMM_TO_REG,
        JUMP,
    } type;
};

extern const std::vector<Encoding> instruction_encodings;

} // namespace sim::decode::table
