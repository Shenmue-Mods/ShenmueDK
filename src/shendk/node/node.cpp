#include "shendk/node/node.h"

#include "shendk/utils/memstream.h"

namespace shendk {

Node::Node() {}
Node::~Node() {}

void Node::read(std::istream& stream) {
    baseOffset = stream.tellg();
    stream.read(reinterpret_cast<char*>(&header), sizeof(Node::Header));

    // check for invalid node
    if (header.signature == 0 || header.size == 0) {
        return;
    }

    uint64_t bufferSize = header.size - sizeof(Node::Header);
    char* buffer = new char[bufferSize];
    stream.read(buffer, static_cast<int64_t>(bufferSize));
    imstream nodeStream(buffer, bufferSize);
    _read(nodeStream);
    delete[] buffer;
    stream.seekg(baseOffset + header.size, std::ios::beg);
}

void Node::write(std::ostream& stream) {
    baseOffset = stream.tellp();
    stream.seekp(sizeof(Node::Header), std::ios::cur);
    _write(stream);
    header.size = stream.tellp() - baseOffset;
    stream.seekp(baseOffset, std::ios::beg);
    stream.write(reinterpret_cast<char*>(&header), sizeof(Node::Header));
    stream.seekp(baseOffset + header.size, std::ios::beg);
}

void Node::_read(std::istream&) {}
void Node::_write(std::ostream&) {}

}
