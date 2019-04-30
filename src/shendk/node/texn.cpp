#include "shendk/node/texn.h"

namespace shendk {

TEXN::TEXN() {}
TEXN::TEXN(std::istream& stream) { read(stream); }
TEXN::~TEXN() {}

void TEXN::_read(std::istream& stream) {
    stream.read(reinterpret_cast<char*>(&textureID), sizeof(TextureID));
    pvrt.read(stream);
}

void TEXN::_write(std::ostream& stream) {
    stream.write(reinterpret_cast<char*>(&textureID), sizeof(TextureID));
    pvrt.write(stream);
}

}
