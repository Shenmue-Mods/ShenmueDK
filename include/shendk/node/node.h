#pragma once

#include <stdint.h>
#include <vector>

#include "shendk/utils/memstream.h"

namespace shendk {

/**
 * @brief Shenmue base node structure.
 */
struct Node {

    struct Header {
        uint32_t signature;
        uint32_t size;
    };

    Node();
    virtual ~Node();

    void read(std::istream& stream);
    void write(std::ostream& stream);

    Node::Header header;

protected:
    virtual void _read(std::istream&);
    virtual void _write(std::ostream&);

    int64_t baseOffset;
};

}
