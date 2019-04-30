#include "shendk/files/container/spr.h"

namespace shendk {

SPR::SPR() = default;
SPR::SPR(std::istream& stream) { read(stream); }
SPR::SPR(const std::string& filepath) { read(filepath); }

void SPR::unpack(const std::string& folder) {
    // TODO: implement
}

void SPR::_read(std::istream& stream) {
    while(!stream.eof()) {
        TEXN entry;
        entry.read(stream);
        entries.push_back(entry);
    }
}

void SPR::_write(std::ostream& stream) {
    for (auto& texn : entries) {
        texn.write(stream);
    }
}

bool SPR::_isValid(uint32_t signature)
{
    return false;
}

}
