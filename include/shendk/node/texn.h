#pragma once

#include <stdint.h>

#include "shendk/node/node.h"
#include "shendk/types/texture_id.h"
#include "shendk/files/image/pvr.h"

namespace shendk {

/**
 * @brief Texture entry node with texture ID and PVR texture.
 */
struct TEXN : public Node {
    TEXN();
    TEXN(std::istream& stream);
    ~TEXN();

    TextureID textureID;
    PVR pvrt;
protected:
    virtual void _read(std::istream& stream);
    virtual void _write(std::ostream& stream);
};

}
