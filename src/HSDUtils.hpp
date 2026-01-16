#pragma once

#include <array>
#include <cstring>
#include <string_view>

template <size_t N>
inline void copyToArray(std::array<char, N>& dest, std::string_view src)
{
    const size_t n = std::min(src.size(), N - 1);
    std::memcpy(dest.data(), src.data(), n);
    dest[n] = '\0';
}
