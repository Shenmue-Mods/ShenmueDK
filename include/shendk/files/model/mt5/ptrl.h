#pragma once

#include "shendk/node/node.h"

namespace shendk {
namespace mt5 {

struct PTRL : Node {
    PTRL();
    PTRL(std::istream& stream);
protected:
    virtual void _read(std::istream& stream);
    virtual void _write(std::ostream& stream);
};

}
}
