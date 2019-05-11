#pragma once

#include <vector>
#include <map>

#include "shendk/files/file.h"

namespace shendk {

struct IPAC : public File {
    const static uint32_t signature = 1128353865;

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

    IPAC();
    IPAC(std::istream& stream);
    IPAC(const std::string& filepath);

    ~IPAC();

    IPAC::Header header;
    std::vector<IPAC::Entry> entries;

protected:
    virtual void _read(std::istream& stream);
    virtual void _write(std::ostream& stream);
    virtual bool _isValid(uint32_t signature);
};
}
