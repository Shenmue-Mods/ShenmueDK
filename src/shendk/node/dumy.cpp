#include "shendk/node/dumy.h"

namespace shendk {

DUMY::DUMY() {
    data.resize(12);
    memset(data.data(), 0, data.size());
}
DUMY::DUMY(std::istream& stream) { read(stream); }
DUMY::~DUMY() {}

void DUMY::_read(std::istream& stream) {
    data.resize(header.size - sizeof(Node::Header));
    stream.read(data.data(), data.size());
}

void DUMY::_write(std::ostream& stream) {
    stream.write(data.data(), data.size());
}

}
