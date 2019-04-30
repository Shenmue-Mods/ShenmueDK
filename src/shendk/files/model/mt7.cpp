#include "shendk/files/model/mt7.h"

namespace shendk {

MT7::MT7() = default;

MT7::MT7(const std::string& filepath) {
    read(filepath);
}

void MT7::_read(std::istream& stream) {
    int64_t baseOffset = stream.tellg();

    std::istream* _stream = &stream;

    _stream->seekg(baseOffset, std::ios::beg);

    // Read header..
    _stream->read(reinterpret_cast<char*>(&header), sizeof(MT7::Header));
}

void MT7::_write(std::ostream& stream) {
    stream.write(reinterpret_cast<char*>(&header), sizeof(MT7::Header));
}

bool MT7::_isValid(uint32_t signature) {
    return false;
}

}
