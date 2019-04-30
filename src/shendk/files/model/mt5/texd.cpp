#include "shendk/files/model/mt5/texd.h"

namespace shendk {
namespace mt5 {

TEXD::TEXD() {}
TEXD::TEXD(std::istream& stream) { read(stream); }

void TEXD::_read(std::istream& stream) {
    stream.read(reinterpret_cast<char*>(&textureCount), sizeof(uint32_t));
}

void TEXD::_write(std::ostream& stream) {
    stream.write(reinterpret_cast<char*>(&textureCount), sizeof(uint32_t));
}

}
}
