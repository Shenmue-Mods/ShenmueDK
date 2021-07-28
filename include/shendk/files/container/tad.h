#pragma once

#include <vector>

#include "shendk/utils/memstream.h"

#include "shendk/utils/sharedmem.h"
#include "shendk/files/file.h"

namespace shendk {

/**
 * @brief d3t indexing file for TAC container.
 */
struct TAD : public File {
public:
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
        unsigned int fileType;
        unsigned int identifier1;
        unsigned int identifier2;
        unsigned int pad0;

        unsigned int unixTimestamp;
        unsigned int pad1;

        unsigned int renderType;
        unsigned int pad2;

        unsigned int headerChecksum;
        unsigned int pad3;

        unsigned int tacSize;
        unsigned int pad4;

        unsigned int fileCount;
        unsigned int pad5;
    };

    TAD();
    TAD(const std::string& filepath);
    ~TAD();

    bool extract(const std::string& tacFilepath, const std::string& outputFolder);
    std::vector<char> readAsset(const std::string& tacFilepath, const std::string& assetPath);
    std::vector<char> openAsset(const std::string& assetPath, const std::string& tacFilepath);

    TAD::Header header;
    std::vector<TAD::Entry> entries;
    sharedmem* tac_memory = nullptr;
    
protected:
    virtual void _read(std::istream& stream);
    virtual void _write(std::ostream& stream);
    virtual bool _isValid(uint32_t signature);
};
}
