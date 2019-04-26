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

    struct EntryMeta {
        char filename[8];
        char extension[4];
        uint32_t fileOffset;
        uint32_t fileSize;

        std::string getFilename() { return std::string(filename, 8); }
        std::string getExtension() { return std::string(extension, 4); }
    };

    struct Entry {

        Entry() {}

        Entry(EntryMeta entryMeta)
            : meta(entryMeta)
        {
            data.resize(meta.fileSize);
        }

        void readData(std::istream& stream) {
            stream.read(data.data(), meta.fileSize);
        }

        void writeData(std::ostream& stream) {
            stream.write(data.data(), meta.fileSize);
        }

        EntryMeta meta;

    private:
        std::vector<char> data;
    };

    IPAC() = default;
    IPAC(std::istream& stream) { read(stream); }
    IPAC(const std::string& filepath) { read(filepath); }

    ~IPAC() {}

    IPAC::Header header;
    std::vector<IPAC::Entry> entries;

protected:
    virtual void _read(std::istream& stream) {
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

    virtual void _write(std::ostream& stream) {
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

	virtual bool _isValid(unsigned int signature)
	{
		if (signature != IPAC::signature)
			return false;
		else return true;
	}
};
}
