#pragma once

#include "shendk/node/node.h"

namespace shendk {
namespace mt5 {

struct PTRL : Node {

    PTRL() {}
    PTRL(std::istream& stream) { read(stream); }

protected:

    void _read(std::istream& stream) {

    }

    void _write(std::ostream& stream) {

    }
};

}
}
