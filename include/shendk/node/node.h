#pragma once

#include <stdint.h>

namespace shendk {

struct Node {

    struct Header {
        char signature[4];
        uint32_t size;
    };

    void serialize() {
        header.size = _serialize(data);
    }

    void deserialize() {

    }

    Node::Header header;

protected:
    virtual uint32_t _serialize(char* data) = 0;
    virtual void _deserialize(char* data) = 0;

private:
    char* data;

};

}
