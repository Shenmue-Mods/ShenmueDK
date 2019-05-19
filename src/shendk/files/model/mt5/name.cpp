#include "shendk/files/model/mt5/name.h"

#include "shendk/types/texture_id.h"

namespace shendk {
namespace mt5 {

NAME::NAME() {}
NAME::NAME(std::istream& stream) { read(stream); }

void NAME::_read(std::istream& stream) {
    while (stream.tellg() < header.size - 8) {
        TextureID texID;
        stream.read(reinterpret_cast<char*>(&texID), sizeof(TextureID));
        textureIDs.push_back(texID);
    }
}

void NAME::_write(std::ostream& stream) {
    for (auto& texID : textureIDs) {
        stream.write(reinterpret_cast<char*>(&texID), sizeof(TextureID));
    }
}

}
}
