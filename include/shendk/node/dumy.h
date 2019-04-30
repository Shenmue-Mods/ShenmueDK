#pragma once

#include <stdint.h>
#include <vector>

#include "shendk/node/node.h"

namespace shendk {

struct DUMY : public Node {
    DUMY();
    DUMY(std::istream& stream);
    virtual ~DUMY();
protected:
    virtual void _read(std::istream& stream);
    virtual void _write(std::ostream& stream);
    std::vector<char> data;
};

}
