#pragma once

#include <stdint.h>
#include <vector>

#include "shendk/node/node.h"

namespace shendk {

struct DUMY : public Node {

    DUMY() {
        data.resize(12);
        memset(data.data(), 0, data.size());
    }
    DUMY(std::istream& stream) { read(stream); }

    virtual ~DUMY() {}

protected:

    void _read(std::istream& stream) {
        data.resize(header.size - sizeof(Node::Header));
        stream.read(data.data(), data.size());
    }

    void _write(std::ostream& stream) {
        stream.write(data.data(), data.size());
    }

    std::vector<char> data;
};

}
