#pragma once

#include <vector>

#include "shendk/node/node.h"
#include "shendk/files/model/mt5/ptrl.h"
#include "shendk/files/model/mt5/texl.h"
#include "shendk/node/texn.h"
#include "shendk/types/texture_id.h"

namespace shendk {
namespace mt5 {

struct NAME : Node {

    NAME() {}
    NAME(std::istream& stream) { read(stream); }

    std::vector<TextureID> textureIDs;

protected:

    void _read(std::istream& stream) {
        while (stream.tellg() < baseOffset + header.size) {
            TextureID texID;
            stream.read(reinterpret_cast<char*>(&texID), sizeof(TextureID));
            textureIDs.push_back(texID);
        }
    }

    void _write(std::ostream& stream) {
        for (auto& texID : textureIDs) {
            stream.write(reinterpret_cast<char*>(&texID), sizeof(TextureID));
        }
    }
};

}
}
