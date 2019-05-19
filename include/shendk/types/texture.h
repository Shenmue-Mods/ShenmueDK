#pragma once

#include <memory>

#include "shendk/types/texture_id.h"
#include "shendk/types/image.h"

namespace shendk {

/**
 * @brief Texture with Shenmue texture ID
 */
struct Texture {
    TextureID textureID;
    std::shared_ptr<Image> image;
};

}
