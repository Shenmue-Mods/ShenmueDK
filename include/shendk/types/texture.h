#pragma once

#include "shendk/types/texture_id.h"
#include "shendk/types/image.h"

namespace shendk {

struct Texture {
    TextureID textureID;
    Image* image = nullptr;
};

}
