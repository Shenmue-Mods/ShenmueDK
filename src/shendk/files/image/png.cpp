#include "shendk/files/image/png.h"

namespace shendk {

PNG::PNG() = default;

PNG::PNG(const std::string& filepath) {
    read(filepath);
}

PNG::~PNG() {}

void PNG::_read(std::istream& stream) {

}

void PNG::_write(std::ostream& stream) {

}

bool PNG::_isValid(uint32_t signature) {
    return false;
}

}
