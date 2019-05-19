#pragma once

#include <string>

namespace shendk {

/**
 * @brief Shenmue texture ID
 */
struct TextureID {
    char id[8];

    bool operator==(const TextureID& other);
    bool operator!=(const TextureID& other);
    std::string hexStr();
};

}
