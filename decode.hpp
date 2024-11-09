#pragma once

#include "common.hpp"

#include "instructions.hpp"

#include <optional>
#include <vector>

namespace sim::decode {

std::optional<instructions::Instruction> try_decode(const std::vector<u8> &memory,
                                                    u8 address) noexcept;
} // namespace sim::decode
