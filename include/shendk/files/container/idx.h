#pragma once

#include <map>

#include "shendk/files/container_file.h"

namespace shendk {

/**
 * @brief Indexing file for AFS.
 */
struct IDX : public File {

    enum class Type {
        IDX0,
        IDXB,
        IDXC,
        IDXD,
        HUMANS
    };

    struct IDX0_Entry {
        char name[12];
        uint16_t afsIndex;
        uint16_t afsLastIndex;
        uint32_t unknown;
    };

    struct IDXB_Entry {
        uint32_t unknown1;
        char name[4];
        uint32_t unknown2;
        uint32_t unknown3;
    };

    struct IDXC_Entry {
        uint32_t unknown1;
        char name[4];
        uint32_t unknown2;
    };

    struct IDXD_Entry {
        uint32_t unknown;
        char name[4];
    };

    struct HUMANS_Entry {
        char name[4];
    };

    IDX();
    IDX(const std::string& filepath);
    IDX(std::istream& stream);
    ~IDX();

    std::string getFilename(uint32_t index);
    IDX::Type getType(uint32_t signature);

    std::vector<std::string> entries;

protected:
    virtual void _read(std::istream& stream);
    virtual void _write(std::ostream& stream);
    virtual bool _isValid(uint32_t signature);
};

}
