#include "shendk/files/container/afs.h"

namespace shendk {

AFS::AFS() = default;
AFS::AFS(const std::string& filepath) { read(filepath); }
AFS::AFS(std::istream& stream) { read(stream); }

AFS::~AFS() {}

void AFS::unpack(const std::string& folder) {
    fs::create_directories(folder);
}

void AFS::mapIdxFilenames(IDX& idx) {
    for (int i = 0; i < entries.size(); i++) {
        entries[i].idxFilename = idx.getFilename(i);
    }
}

void AFS::_read(std::istream& stream) {
    // read header
    stream.read(reinterpret_cast<char*>(&header), sizeof(AFS::Header));

    if (!isValid(header.signature))
        throw new std::runtime_error("Invalid signature for AFS file!\n");

    std::vector<AFS::OffsetEntry> entriesOffset;
    std::vector<AFS::MetaEntry> entriesMeta;

    // read dictionary
    for (uint32_t i = 0; i < header.fileCount; i++) {
        AFS::OffsetEntry entry;
        stream.read(reinterpret_cast<char*>(&entry), sizeof(AFS::OffsetEntry));
        entriesOffset.push_back(entry);
    }

    // calculate meta section offset
    uint32_t metaOffset = 0, metaSize = 0;
    if (header.fileCount > fileCountMagic) {
        stream.seekg(baseOffset + maxPadding - 8, std::ios::beg);
    } else {
        uint32_t curPos = static_cast<uint32_t>(stream.tellg());
        uint32_t offset = curPos + padding - (curPos % padding);
        stream.seekg(baseOffset + offset - 8, std::ios::beg);
    }
    stream.read(reinterpret_cast<char*>(&metaOffset), sizeof(uint32_t));
    stream.read(reinterpret_cast<char*>(&metaSize), sizeof(uint32_t));

    // read entry meta
    if (metaOffset != 0) {
        stream.seekg(baseOffset + metaOffset, std::ios::beg);
        for (uint32_t i = 0; i < header.fileCount; i++) {
            AFS::MetaEntry entry;
            stream.read(reinterpret_cast<char*>(&entry), sizeof(AFS::MetaEntry));
            entriesMeta.push_back(entry);
        }
    }

    // combine offsets with metadata
    for (int i = 0; i < entriesOffset.size(); i++) {
        if (i >= entriesMeta.size()) {
            AFS::Entry entry(entriesOffset[i]);
            entries.push_back(entry);
        } else {
            entries.push_back(AFS::Entry(entriesOffset[i], entriesMeta[i]));
        }
    }

    // read entry data
    for (auto& entry : entries) {
        stream.seekg(baseOffset + entry.offset.fileOffset, std::ios::beg);
        entry.readData(stream);
    }
}

void AFS::_write(std::ostream& stream) {
    // calculate entry data start offset
    uint32_t fileCount = static_cast<uint32_t>(entries.size());
    uint32_t startOffset = sizeof(AFS::Header) + fileCount * sizeof(AFS::OffsetEntry);
    startOffset = startOffset + padding - (startOffset % padding);
    if (fileCount > fileCountMagic) {
        startOffset = maxPadding;
    }

    // calculate entry data offsets
    uint32_t offset = startOffset;
    for (auto& entry : entries) {
        entry.offset.fileOffset = offset;
        offset += entry.offset.fileSize;
        offset += padding - (offset % padding); //sector padding
    }

    // update and write header
    header.fileCount = fileCount;
    stream.write(reinterpret_cast<char*>(&header), sizeof(AFS::Header));

    // write entry offsets
    for (auto& entry : entries) {
        stream.write(reinterpret_cast<char*>(&entry.offset), sizeof(AFS::OffsetEntry));
    }

    // write meta offset
    uint32_t metaOffset = offset;
    uint32_t metaSize = fileCount * sizeof(AFS::MetaEntry);
    stream.seekp(baseOffset + metaOffset - sizeof(AFS::Header), std::ios::beg);
    stream.write(reinterpret_cast<char*>(&metaOffset), sizeof(uint32_t));
    stream.write(reinterpret_cast<char*>(&metaSize), sizeof(uint32_t));

    // write entry data
    for (auto& entry : entries) {
        stream.seekp(baseOffset + entry.offset.fileOffset, std::ios::beg);
        entry.writeData(stream);
    }

    // write meta entries
    stream.seekp(baseOffset + metaOffset, std::ios::beg);
    for (auto& entry : entries) {
        stream.write(reinterpret_cast<char*>(&entry.meta), sizeof(AFS::MetaEntry));
    }
}

bool AFS::_isValid(uint32_t signature)
{
    if (signature != AFS::signature)
        return false;
    else return true;
}

}
