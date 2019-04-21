#pragma once

#include <map>

#include "shendk/files/file.h"

namespace shendk {

struct AFS : File {

    struct Header {
        uint32_t signature;
        uint32_t fileCount;
    };

    struct OffsetEntry {
        uint32_t fileOffset;
        uint32_t fileSize;
    };

    struct MetaEntry {
        char filename[32];
        uint16_t year;
        uint16_t month;
        uint16_t day;
        uint16_t hour;
        uint16_t minute;
        uint16_t second;
        uint32_t fileSize;
    };

    AFS() = default;

    AFS(const std::string& filepath) {
        read(filepath);
    }

    ~AFS() {}

    AFS::Header header;
    std::vector<AFS::OffsetEntry> entries;
    std::vector<AFS::MetaEntry> entriesMeta;
    std::map<AFS::OffsetEntry*, char*> entriesData;

protected:
    const uint32_t padding = 0x0800; // TODO: make padding adjustable
    const uint32_t maxPadding = 0x0008000;
    const uint32_t fileCountMagic = 1016; // TODO: fix this magic number

    virtual void _read(std::istream& stream) {
        int64_t baseOffset = stream.tellg();

        // read header
        stream.read(reinterpret_cast<char*>(&header), sizeof(AFS::Header));

        // read dictionary
        for (uint32_t i = 0; i < header.fileCount; i++) {
            AFS::OffsetEntry entry;
            stream.read(reinterpret_cast<char*>(&entry), sizeof(AFS::OffsetEntry));
            entries.push_back(entry);
        }

        // calculate meta section offset
        uint32_t metaOffset = 0, metaSize = 0;
        if (header.fileCount > fileCountMagic) {
            stream.seekg(baseOffset + maxPadding - 8, std::ios::beg);
        } else {
            uint32_t curPos = static_cast<uint32_t>(stream.tellg());
            uint32_t offset = curPos + maxPadding - (curPos % padding);
            stream.seekg(baseOffset + offset - 8, std::ios::beg);
        }
        stream.read(reinterpret_cast<char*>(metaOffset), sizeof(uint32_t));
        stream.read(reinterpret_cast<char*>(metaSize), sizeof(uint32_t));

        // read entry meta
        if (metaOffset != 0) {
            stream.seekg(baseOffset + metaOffset - 8, std::ios::beg);
            for (uint32_t i = 0; i < header.fileCount; i++) {
                AFS::MetaEntry entry;
                stream.read(reinterpret_cast<char*>(&entry), sizeof(AFS::MetaEntry));
                entriesMeta.push_back(entry);
            }
        }

        // read entry data
        for (auto& entry : entries) {
            stream.seekg(baseOffset + entry.fileOffset, std::ios::beg);
            char* buffer = new char[entry.fileSize];
            stream.read(buffer, entry.fileSize);
            entriesData.insert({&entry, buffer});
        }
    }

    virtual void _write(std::ostream& stream) {
        int64_t baseOffset = stream.tellp();

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
            entry.fileOffset = offset;
            offset += entry.fileSize;
            offset += padding - (offset % padding); //sector padding
        }

        // update and write header
        header.fileCount = fileCount;
        stream.write(reinterpret_cast<char*>(&header), sizeof(AFS::Header));

        // write entry offsets
        for (auto& entry : entries) {
            stream.write(reinterpret_cast<char*>(&entry), sizeof(AFS::OffsetEntry));
        }

        // write meta offset
        uint32_t metaOffset = offset;
        uint32_t metaSize = fileCount * sizeof(AFS::MetaEntry);
        stream.seekp(baseOffset + metaOffset - sizeof(AFS::Header), std::ios::beg);
        stream.write(reinterpret_cast<char*>(&metaOffset), sizeof(uint32_t));
        stream.write(reinterpret_cast<char*>(&metaSize), sizeof(uint32_t));

        // write entry data
        for (auto& entry : entries) {
            stream.seekp(baseOffset + entry.fileOffset, std::ios::beg);
            char* buffer = entriesData[&entry];
            stream.write(buffer, entry.fileSize);
        }

        // write meta entries
        stream.seekp(baseOffset + metaOffset, std::ios::beg);
        for (auto& entry : entriesMeta) {
            stream.write(reinterpret_cast<char*>(&entry), sizeof(AFS::MetaEntry));
        }
    }

};

}
