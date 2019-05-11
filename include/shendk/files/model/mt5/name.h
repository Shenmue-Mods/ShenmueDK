#pragma once

#include <vector>

#include "shendk/node/node.h"
#include "shendk/types/texture_id.h"

namespace shendk {
namespace mt5 {

struct NAME : public Node {
    NAME();
    NAME(std::istream& stream);
    std::vector<TextureID> textureIDs;
protected:
    virtual void _read(std::istream& stream);
    virtual void _write(std::ostream& stream);
};

}
}
