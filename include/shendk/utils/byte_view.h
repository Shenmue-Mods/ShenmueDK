#pragma once

#include <cstddef>
#include <cstdint>
#include <span>

namespace shendk {

using Bytes = std::span<const std::byte>;

constexpr bool Contains(Bytes bytes, std::uint64_t offset, std::uint64_t size) {
    return offset <= bytes.size() && size <= bytes.size() - offset;
}

constexpr std::uint32_t MakeFourCC(char first, char second, char third, char fourth) {
    return static_cast<std::uint32_t>(static_cast<unsigned char>(first)) |
        (static_cast<std::uint32_t>(static_cast<unsigned char>(second)) << 8U) |
        (static_cast<std::uint32_t>(static_cast<unsigned char>(third)) << 16U) |
        (static_cast<std::uint32_t>(static_cast<unsigned char>(fourth)) << 24U);
}

} // namespace shendk
