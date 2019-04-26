#pragma once

#include <map>
#include <set>

#include "shendk/node/node.h"
#include "shendk/node/chrd/chrs.h"
#include "shendk/node/chrd/strg.h"

namespace shendk {

struct CHRD : Node {

    virtual ~CHRD() {}

    chrd::STRG strg;
    chrd::CHRS chrs;

protected:

    virtual void _read(std::istream& stream) {
        // skip chrs
        int64_t chrsOffset = stream.tellg();
        Node::Header chrsHeader;
        stream.read(reinterpret_cast<char*>(&chrsHeader), sizeof(Node::Header));
        stream.seekg(chrsOffset + chrsHeader.size, std::ios::beg);

        // read strg
        strg.read(stream);

        // read chrs
        chrs.strg = &strg;
        stream.seekg(chrsOffset, std::ios::beg);
        chrs.read(stream);
    }

    virtual void _write(std::ostream& stream) {
    }

};

}
