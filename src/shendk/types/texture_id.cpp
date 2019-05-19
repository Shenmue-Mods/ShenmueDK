#include "shendk/types/texture_id.h"

#include <cstring>
#include <iomanip>
#include <sstream>

namespace shendk {

bool TextureID::operator==(const TextureID& other) {
    if (strncmp(id, other.id, 8) == 0) return true;
    return false;
}

bool TextureID::operator!=(const TextureID& other) {
    return !(*this == other);
}

std::string TextureID::hexStr() {
    std::stringstream ss;
    for (int i = 0; i < 8; i++) {
        char buffer[2];
        std::sprintf(&buffer[0], "%02X", static_cast<uint8_t>(id[i]));
        ss << buffer;
    }
    return ss.str();
}

}
