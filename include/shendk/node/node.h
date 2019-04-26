#pragma once

#include <stdint.h>
#include <vector>

#include "shendk/utils/memstream.h"

namespace shendk {

struct Node {

    struct Header {
        uint32_t signature;
        uint32_t size;
    };

    virtual ~Node() {}

    void read(std::istream& stream) {
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

    void write(std::ostream& stream) {
        baseOffset = stream.tellp();
        stream.seekp(sizeof(Node::Header), std::ios::cur);
        _write(stream);
        header.size = stream.tellp() - baseOffset;
        stream.seekp(baseOffset, std::ios::beg);
        stream.write(reinterpret_cast<char*>(&header), sizeof(Node::Header));
        stream.seekp(baseOffset + header.size, std::ios::beg);
    }

    Node::Header header;

protected:
    virtual void _read(std::istream&) {}
    virtual void _write(std::ostream&) {}

    int64_t baseOffset;

};

}
