#pragma once

#include "shendk/utils/byte_view.h"
#include "shendk/utils/murmurhash2.h"
#include "shendk/utils/path_hash.h"

#include <array>
#include <istream>
#include <string_view>

namespace shendk {

struct TadHeader {
    std::uint32_t fileType;
    std::uint32_t version;
    std::uint32_t identifier;
    std::uint32_t flags;
    std::uint64_t order;
    std::array<char, 8> backend;
    std::uint32_t checksum;
    std::uint32_t reserved24;
    std::uint64_t tacSize;
    std::uint64_t fileCount;
    std::uint32_t entryCount;
};

struct TadEntry {
    std::uint32_t hash1;
    std::uint32_t hash2;
    std::uint32_t unknown;
    std::uint32_t reserved0c;
    std::uint32_t fileOffset;
    std::uint32_t reserved14;
    std::uint32_t fileSize;
    std::uint32_t reserved1c;
};
static_assert(sizeof(TadEntry) == 0x20);

bool ReadTadHeader(Bytes bytes, TadHeader &header, std::string_view backend = {}, std::uint32_t type = 2);
bool ReadTadEntry(Bytes bytes, std::uint32_t index, TadEntry &entry);
bool ReadTacAsset(Bytes tac, const TadEntry &entry, std::uint64_t offset, std::span<std::byte> destination);
bool ReadTacAsset(std::istream &tac, const TadEntry &entry, std::uint64_t offset, std::span<std::byte> destination);

} // namespace shendk
