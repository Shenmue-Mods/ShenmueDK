#pragma once

#include <stdint.h>
#include <string>

namespace shendk {

struct MurmurHash2 {

    static uint32_t hashData(const uint8_t* data, uint32_t length);
    static uint32_t hashFilenamePlain(std::string filename, bool toLower);

private:
    static const uint32_t m_initializationSeed = 0x066EE5D0;
    static const uint32_t m_multiplier = 0x5BD1E995;
    static const int32_t m_rotationAmount = 0x18;
    const std::string m_prefix = "./tex/assets/";
};
}
