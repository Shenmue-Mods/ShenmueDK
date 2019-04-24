#pragma once

#include "shendk/node/node.h"

namespace shendk {
namespace mt5 {

struct TEXL : Node {

    struct Data {
        uint32_t unknown;
        uint32_t textureCount;
    };

    TEXL() {}
    TEXL(std::istream& stream) { read(stream); }

    TEXL::Data data;

protected:

    void _read(std::istream& stream) {
        stream.read(reinterpret_cast<char*>(&data), sizeof(TEXL::Data));
    }

    void _write(std::ostream& stream) {
        stream.write(reinterpret_cast<char*>(&data), sizeof(TEXL::Data));
    }
};

}
}
