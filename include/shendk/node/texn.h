#pragma once

#include <stdint.h>

#include "shendk/node/node.h"
#include "shendk/types/texture_id.h"
#include "shendk/files/image/pvr.h"

namespace shendk {

struct TEXN : public Node {

    virtual ~TEXN() {}

    TextureID textureID;
    PVR pvrt;

protected:

    void _write(std::ostream& stream) {
        stream.write(reinterpret_cast<char*>(&textureID), sizeof(TextureID));
        pvrt.write(stream);
    }

    void _read(std::istream& stream) {
        stream.read(reinterpret_cast<char*>(&textureID), sizeof(TextureID));
        pvrt.read(stream);
    }

};

}
