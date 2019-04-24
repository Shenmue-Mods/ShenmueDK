#pragma once

#include <vector>

#include "shendk/node/node.h"
#include "shendk/files/model/mt5/ptrl.h"
#include "shendk/files/model/mt5/texl.h"
#include "shendk/node/texn.h"
#include "shendk/types/texture_id.h"

namespace shendk {
namespace mt5 {

struct TEXD : Node {

    TEXD() {}
    TEXD(std::istream& stream) { read(stream); }

    uint32_t textureCount;

protected:

    void _read(std::istream& stream) {
        stream.read(reinterpret_cast<char*>(&textureCount), sizeof(uint32_t));
    }

    void _write(std::ostream& stream) {
        stream.write(reinterpret_cast<char*>(&textureCount), sizeof(uint32_t));
    }
};

}
}
