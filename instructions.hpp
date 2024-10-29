#pragma once

#include "common.hpp"

#include <string>
#include <vector>

namespace sim::instructions {

// TODO(louis): create a proper abstraction. strings whole zero meaning to the system
struct Instruction {
    std::string op;
    std::string dst;
    std::string src;

    size_t address;
    std::vector<u8> bytes;
};

} // namespace sim::instructions
