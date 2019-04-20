#pragma once

#include <stdint.h>
#include <vector>
#include <map>
#include <iostream>
#include <filesystem>

#include "shendk/files/file.h"
#include "shendk/files/container/gz.h"

namespace fs = std::filesystem;

namespace shendk {

struct PKF : File {

    struct Header {
        uint32_t signature;
        uint32_t contentSize;
        uint32_t unknown;
        uint32_t fileCount;
    };

    struct Entry {
        char token[4];
        uint32_t size;
    };

    PKF() = default;

    PKF(const std::string& filepath) {
        read(filepath);
    }

    ~PKF() {}

    PKF::Header header;
    std::vector<PKF::Entry> entries;
    std::map<PKF::Entry, char*> entriesData;

protected:
    virtual void _read(std::ifstream& stream) {
        int64_t baseOffset = stream.tellg();

        // decompress if necessary
        if (GZ::testGzip(stream)) {
            uint64_t bufferSize;
            char* decompressed = GZ::inflateStream(stream, bufferSize);
            stream.close();
            if (decompressed == nullptr) {
                return;
            }
            stream.open(decompressed, std::ios::binary);
        } else {
            stream.seekg(baseOffset, std::ios::beg);
        }

        // read header
        stream.read(reinterpret_cast<char*>(&header), sizeof(PKF::Header));

        // check for DUMY entry
        PKF::Entry dummyEntry;
        stream.read(reinterpret_cast<char*>(&dummyEntry), sizeof(PKF::Entry));
        if (strncmp(dummyEntry.token, "DUMY", 4) == 0) {
            stream.seekg(dummyEntry.size - sizeof(PKF::Entry), std::ios::cur);
        } else {
            stream.seekg(stream.tellg() - static_cast<int64_t>(sizeof(PKF::Entry)), std::ios::cur);
        }

        // read entries
        for (uint32_t i = 0; i < header.fileCount; i++) {
            if (stream.eof()) break;

            // read entry
            PKF::Entry entry;
            stream.read(reinterpret_cast<char*>(&entry), sizeof(PKF::Entry));
            if (strcmp(entry.token, "") == 0) continue; // fix for broken pkf files
            entries.push_back(entry);

            // read entry data
            uint32_t bufferSize = entry.size - sizeof(PKF::Entry);
            char* buffer = new char[bufferSize];
            stream.read(buffer, bufferSize);
            entriesData.insert({entry, buffer});
        }
    }

    virtual void _write(std::ofstream& stream) {
        int64_t baseOffset = stream.tellp();

        // skip header
        stream.seekp(sizeof(PKF::Header), std::ios::cur);

        // write entries
        for (auto& entry : entries) {
            char* buffer = entriesData[entry];
            stream.write(reinterpret_cast<char*>(&entry), sizeof(PKF::Entry));
            stream.write(buffer, entry.size - sizeof(PKF::Entry));
        }

        // update headers
        header.fileCount = static_cast<uint32_t>(entries.size());
        header.contentSize = static_cast<uint32_t>(stream.tellp() - baseOffset);

        // write header
        stream.seekp(baseOffset, std::ios::beg);
        stream.write(reinterpret_cast<char*>(&header), sizeof(PKF::Header));
    }
};
}
