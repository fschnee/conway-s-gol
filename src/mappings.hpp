#pragma once

#include <tuple> // std::pair

#include "type_aliases.hpp"

auto map_1d_to_2d(u64 index, u64 width) -> std::pair<u64 /*x*/, u64 /*y*/>
{
    auto y = index / width; // Rounds down.
    auto x = index - y * width;
    return { x, y };
}

auto map_2d_to_1d(u64 x, u64 y, u64 width) -> u64 { return y * width + x; }
