#include "shendk/files/container/tad.h"
#include "shendk/files/container/tad_reader.h"
#include <array>
#include <cstring>
#include <stdexcept>


#include "shendk/utils/murmurhash2.h"
#include "shendk/utils/hash_db.h"

namespace shendk {

TAD::TAD() = default;

TAD::TAD(const std::string& filepath) {
    read(filepath);
}

TAD::~TAD() {
    if (tac_memory) {
        tac_memory->CloseSharedMemory();
        delete tac_memory;
    }
}

std::vector<char> TAD::readAsset(const std::string& tacFilepath, const std::string& assetPath) {
    if (tacFilepath.empty() || assetPath.empty())
        return {};
    const uint64_t hash = shendk::GetPathHash(assetPath);
    const Entry *selected = nullptr;
    for (const Entry &entry : entries) {
        if (entry.hash1 == static_cast<uint32_t>(hash >> 32) && entry.hash2 == static_cast<uint32_t>(hash)) {
            selected = &entry;
            break;
        }
    }
    if (!selected) {
        HashDB &db = HashDB::getInstance();
        for (const Entry &entry : entries) {
            if (assetPath == db.getFilepath(entry.hash1, entry.hash2)) {
                selected = &entry;
                break;
            }
        }
    }
    if (!selected)
        return {};
    shendk::TadEntry entry;
    static_assert(sizeof(entry) == sizeof(*selected));
    std::memcpy(&entry, selected, sizeof(entry));
    std::ifstream input(tacFilepath, std::ios::binary);
    std::vector<char> result(entry.fileSize);
    if (!shendk::ReadTacAsset(input, entry, 0, std::as_writable_bytes(std::span(result))))
        return {};
    return result;
}

std::vector<char> TAD::openAsset(const std::string& assetPath, const std::string& tacFilepath) {
    return readAsset(tacFilepath, assetPath);
}

bool TAD::extract(const std::string& tacFilepath, const std::string& outputFolder) {
    std::ifstream input(tacFilepath, std::ios::binary);
    if (!input)
        return false;
    fs::path directory(outputFolder);
    fs::create_directories(directory);
    HashDB &db = HashDB::getInstance();
    uint32_t index = 0;
    for (const Entry &source : entries) {
        std::string name = db.getFilepath(source.hash1, source.hash2);
        if (name.empty())
            name = std::to_string(index);
        fs::path relative = fs::path(name).make_preferred().relative_path().lexically_normal();
        for (const fs::path &component : relative)
            if (component == "..")
                return false;
        const fs::path destination = directory / relative;
        fs::create_directories(destination.parent_path());
        shendk::TadEntry entry;
        std::memcpy(&entry, &source, sizeof(entry));
        std::vector<char> data(entry.fileSize);
        if (!shendk::ReadTacAsset(input, entry, 0, std::as_writable_bytes(std::span(data))))
            return false;
        std::ofstream output(destination, std::ios::binary);
        output.write(data.data(), static_cast<std::streamsize>(data.size()));
        if (!output)
            return false;
        ++index;
    }
    return true;
}

void TAD::_read(std::istream& stream) {
    entries.clear();
    header = {};
    std::array<std::byte, 60> raw;
    stream.read(reinterpret_cast<char *>(raw.data()), raw.size());
    shendk::TadHeader decoded;
    if (!stream || !shendk::ReadTadHeader(raw, decoded))
        throw std::runtime_error("Invalid TAD header");
    std::memcpy(&header, raw.data(), sizeof(header));
    const std::streampos entryBegin = stream.tellg();
    stream.seekg(0, std::ios::end);
    const std::streampos end = stream.tellg();
    if (entryBegin < 0 || end < entryBegin || decoded.entryCount > static_cast<uint64_t>(end - entryBegin) / sizeof(Entry))
        throw std::runtime_error("Truncated TAD entry table");
    stream.seekg(entryBegin);
    entries.resize(decoded.entryCount);
    if (!entries.empty())
        stream.read(reinterpret_cast<char *>(entries.data()), static_cast<std::streamsize>(entries.size() * sizeof(Entry)));
    if (!stream)
        throw std::runtime_error("Truncated TAD entry table");
}

void TAD::_write(std::ostream& stream) {
    header.fileCount = static_cast<uint32_t>(entries.size());
    header.headerChecksum = 0;
    header.headerChecksum = MurmurHash2::hashData(reinterpret_cast<uint8_t*>(&header), sizeof(TAD::Header));
    stream.write(reinterpret_cast<char*>(&header), sizeof(TAD::Header));
    stream.write(reinterpret_cast<char*>(&header.fileCount), sizeof(uint32_t));
    for (uint32_t i = 0; i < header.fileCount; i++) {
        stream.write(reinterpret_cast<char*>(&entries[i]), sizeof(TAD::Entry));
    }
}

bool TAD::_isValid(uint32_t signature) {
    return false;
}

}
