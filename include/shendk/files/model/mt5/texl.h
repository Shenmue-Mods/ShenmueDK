#pragma once

#include <vector>

#include "shendk/node/node.h"

namespace shendk {
namespace mt5 {

struct TEXL : Node {

    struct Header {
        uint32_t unknown;
        uint32_t textureCount;
    };

    struct Entry {
        uint32_t textureIDOffset;
        uint32_t unknown1;
        uint32_t unknown2;
    };

    TEXL() {}
    TEXL(std::istream& stream) { read(stream); }

    TEXL::Header texlHeader;
    std::vector<TEXL::Entry> entries;

protected:

    void _read(std::istream& stream) {
        stream.read(reinterpret_cast<char*>(&texlHeader), sizeof(TEXL::Header));
        for (int i = 0; i < texlHeader.textureCount; i++) {
            TEXL::Entry entry;
            stream.read(reinterpret_cast<char*>(&entry), sizeof(TEXL::Entry));
            entries.push_back(entry);
        }
    }

    void _write(std::ostream& stream) {
        stream.write(reinterpret_cast<char*>(&texlHeader), sizeof(TEXL::Header));
        for (auto& entry : entries) {
            stream.write(reinterpret_cast<char*>(&entry), sizeof(TEXL::Entry));
        }
    }
};

}
}
