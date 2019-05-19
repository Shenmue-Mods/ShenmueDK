#pragma once

#include <stdint.h>

#include "shendk/node/node.h"

namespace shendk {

/**
 * @brief Shenmue scene script version 3 node.
 */
struct SCN3 : public Node {
    SCN3();
    SCN3(std::istream& stream);
    ~SCN3();

protected:
    virtual void _read(std::istream& stream);
    virtual void _write(std::ostream& stream);
};

}
