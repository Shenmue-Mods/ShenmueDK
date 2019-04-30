#include "shendk/files/map/mapinfo.h"

namespace shendk {

MAPINFO::MAPINFO() = default;
MAPINFO::MAPINFO(const std::string& filepath) { read(filepath); }
MAPINFO::~MAPINFO() {}

void MAPINFO::_read(std::istream& stream) {

}

void MAPINFO::_write(std::ostream& stream) {

}

bool MAPINFO::_isValid(uint32_t signature) {
    return false;
}

}
