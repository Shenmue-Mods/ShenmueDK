#include "shendk/node/chrd.h"

namespace shendk {

CHRD::~CHRD() {}

void CHRD::_read(std::istream& stream) {
    // skip chrs
    int64_t chrsOffset = stream.tellg();
    Node::Header chrsHeader;
    stream.read(reinterpret_cast<char*>(&chrsHeader), sizeof(Node::Header));
    stream.seekg(chrsOffset + chrsHeader.size, std::ios::beg);

    // read strg
    strg.read(stream);

    // read chrs
    chrs.strg = &strg;
    stream.seekg(chrsOffset, std::ios::beg);
    chrs.read(stream);
}

void CHRD::_write(std::ostream& stream) {
}

}
