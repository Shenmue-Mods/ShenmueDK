#pragma once

#include <map>
#include <set>

#include "shendk/node/node.h"
#include "shendk/node/chrd/chrs.h"
#include "shendk/node/chrd/strg.h"

namespace shendk {

/**
 * @brief Character data node
 */
struct CHRD : public Node {
    virtual ~CHRD();
    chrd::STRG strg;
    chrd::CHRS chrs;
protected:
    virtual void _read(std::istream& stream);
    virtual void _write(std::ostream& stream);
};

}
