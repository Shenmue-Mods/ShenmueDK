#pragma once

#include <vector>

#include "shendk/files/file.h"

namespace shendk {

/**
 * @brief d3t indexing file for TAC container.
 */
struct TAD : public File {

    struct Entry {
        uint32_t hash1;
        uint32_t hash2;
        uint32_t unknown;
        uint32_t pad12;
        uint32_t fileOffset;
        uint32_t pad20;
        uint32_t fileSize;
        uint32_t pad28;
    };

    struct Header {
        uint32_t fileType;
        uint32_t identifier1;
        uint32_t identifier2;
        uint32_t pad12;
        __time32_t unixTimestamp;
        uint32_t pad20;
        char renderType[4];
        uint32_t pad28;
        uint32_t headerChecksum;
        uint32_t pad36;
        uint32_t tacSize;
        uint32_t pad44;
        uint32_t fileCount;
        uint32_t pad52;
    };

    TAD();
    TAD(const std::string& filepath);
    ~TAD();

    bool extract(const std::string& tacFilepath, const std::string& outputFolder);

    TAD::Header header;
    std::vector<TAD::Entry> entries;

protected:
    virtual void _read(std::istream& stream);
    virtual void _write(std::ostream& stream);
    virtual bool _isValid(uint32_t signature);
};
}
