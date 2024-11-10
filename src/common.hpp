#pragma once

#include <cassert>
#include <cstdint>

// TODO(louis): improve debug information, e.g. __LINE__
#define UNREACHABLE() assert(!"Unreachable.");

using u8 = std::uint8_t;
using u16 = std::uint16_t;
