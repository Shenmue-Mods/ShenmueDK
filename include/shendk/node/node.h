#pragma once

#include <stdint.h>

#include "shendk/utils/memstream.h"

namespace shendk {

struct Node {

    struct Header {
        char signature[4];
        uint32_t size;
    };

    void serialize(std::ostream& stream) {
        //header.size = _serialize(data);
    }

    void deserialize(std::istream& stream) {
        stream.read(reinterpret_cast<char*>(&header), sizeof(Node::Header));

        uint64_t bufferSize = header.size - sizeof(Node::Header);
        char* buffer = new char[bufferSize];
        stream.read(buffer, static_cast<int64_t>(bufferSize));

        imstream nodeStream(buffer, bufferSize);
        _deserialize(nodeStream);
        delete[] buffer;
    }

    Node::Header header;

protected:
    virtual uint32_t _serialize(std::ostream& stream) = 0;
    virtual void _deserialize(std::istream& stream) = 0;

};

}
