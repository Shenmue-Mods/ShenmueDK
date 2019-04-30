#pragma once

#include <map>
#include <set>

#include "shendk/node/node.h"

namespace shendk {
namespace chrd {

struct STRG : Node {

    virtual ~STRG();

    int64_t getOffset(const std::string& str);
    std::string getString(int64_t offset);

    std::set<std::string> strings;

protected:
    virtual void _read(std::istream& stream);
    virtual void _write(std::ostream& stream);

    std::set<int64_t> offsets;
};

}
}
