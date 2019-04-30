#include "shendk/files/container/ipac.h"

namespace shendk {

IPAC::IPAC() = default;
IPAC::IPAC(std::istream& stream) { read(stream); }
IPAC::IPAC(const std::string& filepath) { read(filepath); }

IPAC::~IPAC() {}

void IPAC::_read(std::istream& stream) {
    // read header
    stream.read(reinterpret_cast<char*>(&header), sizeof(IPAC::Header));

    if (!isValid(header.signature))
        throw new std::runtime_error("Invalid signature for IPAC file!\n");

    // read dictionary
    stream.seekg(baseOffset + header.dictionaryOffset, std::ios::beg);
    for (uint32_t i = 0; i < header.fileCount; i++) {
        IPAC::EntryMeta entry;
        stream.read(reinterpret_cast<char*>(&entry), sizeof(IPAC::EntryMeta));
        entries.push_back(IPAC::Entry(entry));
    }

    // read entry data
    for (auto& entry : entries) {
        stream.seekg(baseOffset + entry.meta.fileOffset, std::ios::beg);
        entry.readData(stream);
    }
}

void IPAC::_write(std::ostream& stream) {
    // calculate offsets
    uint32_t fileOffset = 16;
    for (auto& entry : entries) {
        entry.meta.fileOffset = fileOffset;
        fileOffset += entry.meta.fileSize;
        fileOffset += 16 - (fileOffset % 16);
        fileOffset += 16; // 16 byte padding
    }

    // update header
    header.contentSize = fileOffset;
    header.dictionaryOffset = fileOffset;
    header.fileCount = static_cast<uint32_t>(entries.size());

    // write header
    stream.write(reinterpret_cast<char*>(&header), sizeof(IPAC::Header));

    // write entry data
    for (auto& entry : entries) {
        stream.seekp(baseOffset + entry.meta.fileOffset, std::ios::beg);
        entry.writeData(stream);
    }

    // write dictionary
    stream.seekp(baseOffset + header.dictionaryOffset, std::ios::beg);
    for (auto& entry : entries) {
        stream.write(reinterpret_cast<char*>(&entry.meta), sizeof(IPAC::EntryMeta));
    }
}

bool IPAC::_isValid(uint32_t signature) {
    if (signature != IPAC::signature)
        return false;
    else return true;
}

}
