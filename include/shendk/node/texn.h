#pragma once

#include <stdint.h>

#include "shendk/node/node.h"
#include "shendk/types/texture_id.h"

namespace shendk {

struct TEXN : public Node {
    TextureID textureID;

};

}
