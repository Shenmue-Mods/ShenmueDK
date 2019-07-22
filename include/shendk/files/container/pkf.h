#pragma once

#include <vector>
#include <map>

#include "shendk/files/file.h"
#include "shendk/files/container/ipac.h"
#include "shendk/node/texn.h"

namespace shendk {

/**
 * @brief Shenmue texture container file made up of TEXN entries.
 */
struct PKF : public File {

    const static uint32_t signature = 1179337040;

    struct Header {
        uint32_t signature;
        uint32_t contentSize;
        uint32_t unknown;
        uint32_t fileCount;
    };

    PKF();
    PKF(std::istream& stream);
    PKF(const std::string& filepath);

    ~PKF();

    PKF::Header header;
    std::vector<TEXN> textures;
    IPAC* ipac = nullptr;

    bool Compressed = false;

protected:
    virtual void _read(std::istream& stream);
    virtual void _write(std::ostream& stream);
    virtual bool _isValid(uint32_t signature);
};
}
