#pragma once

#include "instructions.hpp"

#include <vector>

namespace sim::print {

void instructions(const std::vector<instructions::Instruction> &instructions) noexcept;

} // namespace sim::print
