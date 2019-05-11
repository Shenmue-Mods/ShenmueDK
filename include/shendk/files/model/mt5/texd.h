#pragma once

#include <vector>

#include "shendk/node/node.h"

namespace shendk {
namespace mt5 {

struct TEXD : public Node {
    TEXD();
    TEXD(std::istream& stream);
    uint32_t textureCount;
protected:
    void _read(std::istream& stream);
    void _write(std::ostream& stream);
};

}
}
