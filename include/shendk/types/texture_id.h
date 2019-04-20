#pragma once

#include <cstring>

namespace shendk {

struct TextureID {
    char id[8];

    bool operator==(const TextureID& other) {
        if (strncmp(id, other.id, 8) == 0) return true;
        return false;
    }

    bool operator!=(const TextureID& other) {
        return !(*this == other);
    }

};

}
