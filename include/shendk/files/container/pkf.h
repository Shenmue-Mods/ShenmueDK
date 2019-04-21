#pragma once

#include <stdint.h>
#include <vector>
#include <map>
#include <iostream>
#include <filesystem>

#include "shendk/files/file.h"
#include "shendk/files/container/gz.h"
#include "shendk/utils/memstream.h"

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
    std::map<PKF::Entry*, char*> entriesData;

protected:
    virtual void _read(std::istream& stream) {
        int64_t baseOffset = stream.tellg();

        std::istream* _stream = &stream;

        // decompress if necessary
        if (GZ::testGzip(stream)) {
            uint64_t bufferSize;
            char* decompressed = GZ::inflateStream(stream, bufferSize);
            if (decompressed == nullptr) {
                return;
            }
            _stream = new imstream(decompressed, bufferSize);
        } else {
            _stream->seekg(baseOffset, std::ios::beg);
        }

        // read header
        _stream->read(reinterpret_cast<char*>(&header), sizeof(PKF::Header));

        // check for DUMY entry
        PKF::Entry dummyEntry;
        _stream->read(reinterpret_cast<char*>(&dummyEntry), sizeof(PKF::Entry));
        if (strncmp(dummyEntry.token, "DUMY", 4) == 0) {
            _stream->seekg(dummyEntry.size - sizeof(PKF::Entry), std::ios::cur);
        } else {
            _stream->seekg(_stream->tellg() - static_cast<int64_t>(sizeof(PKF::Entry)), std::ios::cur);
        }

        // read entries
        for (uint32_t i = 0; i < header.fileCount; i++) {
            if (_stream->eof()) break;

            // read entry
            PKF::Entry entry;
            _stream->read(reinterpret_cast<char*>(&entry), sizeof(PKF::Entry));
            if (strcmp(entry.token, "") == 0) continue; // fix for broken pkf files
            entries.push_back(entry);

            // read entry data
            uint32_t bufferSize = entry.size - sizeof(PKF::Entry);
            char* buffer = new char[bufferSize];
            _stream->read(buffer, bufferSize);
            entriesData.insert({&entry, buffer});
        }
    }

    virtual void _write(std::ostream& stream) {
        int64_t baseOffset = stream.tellp();

        // skip header
        stream.seekp(sizeof(PKF::Header), std::ios::cur);

        // write entries
        for (auto& entry : entries) {
            char* buffer = entriesData[&entry];
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
