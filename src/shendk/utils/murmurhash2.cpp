#include "shendk/utils/murmurhash2.h"
#include <cstddef>
#include <cstring>

#include <algorithm>

namespace shendk {

std::uint32_t HashData(const void *source, std::uint32_t size) {
    if (!source && size)
        return 0;
    const std::byte *const data = static_cast<const std::byte *>(source);
    constexpr std::uint32_t multiplier = 0x5bd1e995;
    std::uint32_t hash = (size + size / 0xffffffffU) ^ 0x066ee5d0U;
    std::uint32_t offset = 0;
    while (size - offset >= 4) {
        std::uint32_t word;
        std::memcpy(&word, data + offset, sizeof(word));
        word *= multiplier;
        word ^= word >> 24;
        word *= multiplier;
        hash = hash * multiplier ^ word;
        offset += 4;
    }
    switch (size - offset) {
    case 3:
        hash ^= std::to_integer<std::uint32_t>(data[offset + 2]) << 16;
        [[fallthrough]];
    case 2:
        hash ^= std::to_integer<std::uint32_t>(data[offset + 1]) << 8;
        [[fallthrough]];
    case 1:
        hash = multiplier * (hash ^ std::to_integer<std::uint32_t>(data[offset]));
        break;
    default:
        break;
    }
    hash = multiplier * (hash ^ (hash >> 13));
    return hash ^ (hash >> 15);
}

uint32_t MurmurHash2::hashData(const uint8_t* data, uint32_t length) {
    return HashData(data, length);
}

uint32_t MurmurHash2::hashFilenamePlain(std::string filename, bool toLower) {
    if (toLower) {
        std::transform(filename.begin(), filename.end(), filename.begin(), ::tolower);
    }
    uint32_t length = filename.length();
    const uint8_t* data = reinterpret_cast<const uint8_t*>(&filename[0]);
    uint32_t hash = hashData(data, length);
    return hash;
}

}
