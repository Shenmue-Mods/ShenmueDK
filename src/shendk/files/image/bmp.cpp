#include "shendk/files/image/bmp.h"

#include "shendk/files/image_file.h"

namespace shendk {

BMP::BMP() = default;
BMP::BMP(const std::string& filepath) { read(filepath); }
BMP::~BMP() {}

void BMP::_read(std::istream& stream) {

}

void BMP::_write(std::ostream& stream) {

}

bool BMP::_isValid(uint32_t signature) {
    return false;
}

}
