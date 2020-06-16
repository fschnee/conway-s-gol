#pragma once

#include <cstdint> // std::XintW_t

using u8  = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

using i8  = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;

using f32 = float;
static_assert(sizeof(f32) * 8 == 32);
using f64 = double;
static_assert(sizeof(f64) * 8 == 64);
using f128 = long double;
static_assert(sizeof(f128) * 8 == 128);
