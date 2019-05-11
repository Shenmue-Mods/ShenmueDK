#pragma once

#include <vector>

#include "shendk/node/node.h"

namespace shendk {
namespace mt5 {

struct TEXL : public Node {

    struct Header {
        uint32_t unknown;
        uint32_t textureCount;
    };

    struct Entry {
        uint32_t textureIDOffset;
        uint32_t unknown1;
        uint32_t unknown2;
    };

    TEXL();
    TEXL(std::istream& stream);

    TEXL::Header texlHeader;
    std::vector<TEXL::Entry> entries;

protected:
    virtual void _read(std::istream& stream);
    virtual void _write(std::ostream& stream);
};

}
}
