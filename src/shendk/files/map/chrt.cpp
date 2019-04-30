#include "shendk/files/map/chrt.h"

namespace shendk {

CHRT::CHRT() = default;
CHRT::CHRT(const std::string& filepath) { read(filepath); }
CHRT::~CHRT() {}

void CHRT::_read(std::istream& stream) {
    // skip chrs
    Node::Header chrsHeader;
    stream.read(reinterpret_cast<char*>(&chrsHeader), sizeof(Node::Header));
    stream.seekg(baseOffset + chrsHeader.size, std::ios::beg);

    // read strg
    strg.read(stream);

    // read chrs
    chrs.strg = &strg;
    stream.seekg(baseOffset, std::ios::beg);
    chrs.read(stream);
}

void CHRT::_write(std::ostream& stream) {
    throw new std::runtime_error("Not implemented");
}
bool CHRT::_isValid(uint32_t signature) {
    return false;
}

}
