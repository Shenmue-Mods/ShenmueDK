#pragma once

#include <map>
#include <vector>

#include "shendk/files/container_file.h"
#include "shendk/files/container/idx.h"

namespace shendk {

struct AFS : public ContainerFile {
    const static uint32_t signature = 5457473;

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

    AFS();
    AFS(const std::string& filepath);
    AFS(std::istream& stream);
    ~AFS();

    virtual void unpack(const std::string& folder);
    void mapIdxFilenames(IDX& idx);

    AFS::Header header;
    std::vector<AFS::Entry> entries;

protected:
    const uint32_t padding = 0x0800; // TODO: make padding adjustable
    const uint32_t maxPadding = 0x0008000;
    const uint32_t fileCountMagic = 1016; // TODO: fix this magic number

    virtual void _read(std::istream& stream);
    virtual void _write(std::ostream& stream);
    virtual bool _isValid(uint32_t signature);
};

}
