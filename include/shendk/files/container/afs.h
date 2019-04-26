#pragma once

#include <map>

#include "shendk/files/container_file.h"
#include "shendk/files/container/idx.h"

namespace shendk {

struct AFS : ContainerFile {
	unsigned int signature = 5457473;

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

    struct Entry {

        Entry() {}

        Entry(OffsetEntry offsetEntry)
            : offset(offsetEntry)
        {
            data.resize(offsetEntry.fileSize);
        }

        Entry(OffsetEntry offsetEntry, MetaEntry metaEntry)
            : offset(offsetEntry)
            , meta(metaEntry)
        {
            data.resize(offsetEntry.fileSize);
        }

        void readData(std::istream& stream) {
            stream.read(data.data(), offset.fileSize);
        }

        void writeData(std::ostream& stream) {
            stream.write(data.data(), offset.fileSize);
        }

        OffsetEntry offset;
        MetaEntry meta;
        std::string idxFilename;

    private:
        std::vector<char> data;
    };

    AFS() = default;
    AFS(const std::string& filepath) { read(filepath); }
    AFS(std::istream& stream) { read(stream); }

    ~AFS() {}

    void unpack(const std::string& folder) {
        fs::create_directories(folder);
    }

    void mapIdxFilenames(IDX& idx) {
        for (int i = 0; i < entries.size(); i++) {
            entries[i].idxFilename = idx.getFilename(i);
        }
    }

    AFS::Header header;
    std::vector<AFS::Entry> entries;

protected:
    const uint32_t padding = 0x0800; // TODO: make padding adjustable
    const uint32_t maxPadding = 0x0008000;
    const uint32_t fileCountMagic = 1016; // TODO: fix this magic number

    virtual void _read(std::istream& stream) {
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

    virtual void _write(std::ostream& stream) {
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

	virtual bool _isValid(unsigned int signature)
	{
		if (signature != AFS::signature)
			return false;
		else return true;
	}
};

}
