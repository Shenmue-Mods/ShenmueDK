#include "shendk/files/container/tad.h"

#include "shendk/utils/murmurhash2.h"
#include "shendk/utils/hash_db.h"

namespace shendk {

TAD::TAD() = default;

TAD::TAD(const std::string& filepath) {
    read(filepath);
}

TAD::~TAD() {}

bool TAD::extract(const std::string& tacFilepath, const std::string& outputFolder) {
    if (!fs::exists(tacFilepath)) return false;
    fs::path dir(outputFolder);
    fs::create_directories(dir);

    std::ifstream inStream;
    std::ofstream outStream;
    inStream.open(tacFilepath, std::ios::binary);
    HashDB& db = HashDB::getInstance();
    uint32_t idx = 0;
    for (auto& entry : entries) {
        std::string filepath = db.getFilepath(entry.hash1, entry.hash2);
        if (filepath.empty()) {
            filepath = std::to_string(idx);
        }
        fs::path fullPath(dir);
        fullPath /= fs::path(filepath).make_preferred().relative_path();
        fs::create_directories(fullPath.parent_path());
        char* buffer = new char[entry.fileSize]; // TODO: allocation per entry is kinda slow, maybe use shared buffer.
        outStream.open(fullPath, std::ios::binary);
        inStream.seekg(entry.fileOffset);
        inStream.read(buffer, entry.fileSize);
        outStream.write(buffer, entry.fileSize);
        outStream.close();
        delete[] buffer;
        ++idx;
    }
    inStream.close();
    return true;
}

void TAD::_read(std::istream& stream) {
    stream.read(reinterpret_cast<char*>(&header), sizeof(TAD::Header));
    stream.seekg(4, std::ios::cur); // skip redundant file count
    for (uint32_t i = 0; i < header.fileCount; i++) {
        TAD::Entry entry;
        stream.read(reinterpret_cast<char*>(&entry), sizeof(TAD::Entry));
        entries.push_back(entry);
    }
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
