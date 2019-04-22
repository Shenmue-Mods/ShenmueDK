#pragma once

#include <stdint.h>
#include <vector>
#include <map>
#include <iostream>
#include <filesystem>

#include "shendk/files/file.h"

namespace fs = std::filesystem;

namespace shendk {

struct IPAC : File {
	unsigned int signature = 1128353865;

    struct Header {
        uint32_t signature;
        uint32_t dictionaryOffset;
        uint32_t fileCount;
        uint32_t contentSize;
    };

    struct Entry {
        char filename[8];
        char extension[4];
        uint32_t fileOffset;
        uint32_t fileSize;
    };

    IPAC() = default;

    IPAC(const std::string& filepath) {
        read(filepath);
    }

    ~IPAC() {}

    IPAC::Header header;
    std::vector<IPAC::Entry> entries;
    std::map<IPAC::Entry*, char*> entriesData;

protected:
    virtual void _read(std::istream& stream) {
        int64_t baseOffset = stream.tellg();

        // read header
        stream.read(reinterpret_cast<char*>(&header), sizeof(IPAC::Header));

		if (!isValid(header.signature))
			throw new std::runtime_error("Invalid signature for IPAC file!\n");

        // read dictionary
        stream.seekg(header.dictionaryOffset, std::ios::beg);
        for (uint32_t i = 0; i < header.fileCount; i++) {
            IPAC::Entry entry;
            stream.read(reinterpret_cast<char*>(&entry), sizeof(IPAC::Entry));
            entries.push_back(entry);
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

        // calculate offsets
        uint32_t fileOffset = 16;
        for (auto& entry : entries) {
            entry.fileOffset = fileOffset;
            fileOffset += entry.fileSize;
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
            stream.seekp(baseOffset + entry.fileOffset, std::ios::beg);
            char* buffer = entriesData[&entry];
            stream.write(buffer, entry.fileSize);
        }

        // write dictionary
        stream.seekp(baseOffset + header.dictionaryOffset, std::ios::beg);
        for (auto& entry : entries) {
            stream.write(reinterpret_cast<char*>(&entry), sizeof(IPAC::Entry));
        }
    }

	virtual bool _isValid(unsigned int signature)
	{
		if (signature != IPAC::signature)
			return false;
		else return true;
	}
};
}
