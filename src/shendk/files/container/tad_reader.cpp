#include "shendk/files/container/tad_reader.h"
#include "../reader.h"

#include <cstring>
#include <limits>

namespace shendk {

bool ReadTadHeader(Bytes bytes, TadHeader &header, std::string_view backend, std::uint32_t type) {
    header = {};
    if (!Contains(bytes, 0, 60) || backend.size() > 8)
        return false;
    std::array<std::byte, 56> checksumBytes;
    std::memcpy(checksumBytes.data(), bytes.data(), checksumBytes.size());
    std::uint32_t checksum;
    Read(bytes, 32, checksum);
    std::memset(checksumBytes.data() + 32, 0, sizeof(checksum));
    if (HashData(checksumBytes.data(), static_cast<std::uint32_t>(checksumBytes.size())) != checksum)
        return false;
    Read(bytes, 0, header.fileType);
    Read(bytes, 4, header.version);
    Read(bytes, 8, header.identifier);
    Read(bytes, 12, header.flags);
    Read(bytes, 16, header.order);
    Read(bytes, 24, header.backend);
    Read(bytes, 32, header.checksum);
    Read(bytes, 36, header.reserved24);
    Read(bytes, 40, header.tacSize);
    Read(bytes, 48, header.fileCount);
    Read(bytes, 56, header.entryCount);
    if (header.version < 5 || header.identifier < 2 || (type != 2 && !(header.flags & 0x0aU)))
        return false;
    if (!backend.empty()) {
        std::array<char, 8> expected{};
        std::memcpy(expected.data(), backend.data(), backend.size());
        if (std::strncmp(header.backend.data(), expected.data(), expected.size()) != 0)
            return false;
    }
    return true;
}

bool ReadTadEntry(Bytes bytes, std::uint32_t index, TadEntry &entry) {
    return Read(bytes, 60ULL + static_cast<std::uint64_t>(index) * sizeof(entry), entry);
}

bool ReadTacAsset(Bytes tac, const TadEntry &entry, std::uint64_t offset, std::span<std::byte> destination) {
    if (offset > entry.fileSize || destination.size() > entry.fileSize - offset ||
        !Contains(tac, static_cast<std::uint64_t>(entry.fileOffset) + offset, destination.size()))
        return false;
    if (!destination.empty())
        std::memcpy(destination.data(), tac.data() + entry.fileOffset + static_cast<std::size_t>(offset), destination.size());
    return true;
}

bool ReadTacAsset(std::istream &tac, const TadEntry &entry, std::uint64_t offset, std::span<std::byte> destination) {
    if (offset > entry.fileSize || destination.size() > entry.fileSize - offset ||
        destination.size() > static_cast<std::uint64_t>((std::numeric_limits<std::streamsize>::max)()))
        return false;
    tac.clear();
    tac.seekg(static_cast<std::streamoff>(static_cast<std::uint64_t>(entry.fileOffset) + offset), std::ios::beg);
    if (!tac)
        return false;
    if (destination.empty())
        return true;
    tac.read(reinterpret_cast<char *>(destination.data()), static_cast<std::streamsize>(destination.size()));
    return tac.gcount() == static_cast<std::streamsize>(destination.size());
}

} // namespace shendk
