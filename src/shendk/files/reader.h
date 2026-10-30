#pragma once

#include "shendk/utils/byte_view.h"

#include <bit>
#include <cstring>

namespace {

template <typename T> bool Read(shendk::Bytes bytes, std::uint64_t offset, T &value) {
    static_assert(std::endian::native == std::endian::little);
    if (!shendk::Contains(bytes, offset, sizeof(T)))
        return false;
    std::memcpy(&value, bytes.data() + static_cast<std::size_t>(offset), sizeof(T));
    return true;
}

} // namespace
