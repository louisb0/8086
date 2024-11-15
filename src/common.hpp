#pragma once

#include <cassert>
#include <cstdint>

#define UNREACHABLE() assert(!"Unreachable.");

using u8 = std::uint8_t;
using u16 = std::uint16_t;

using s8 = std::int8_t;
using s16 = std::int16_t;
