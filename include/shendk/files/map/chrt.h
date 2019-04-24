#pragma once

#include "shendk/files/file.h"
#include "shendk/node/chrd/chrs.h"
#include "shendk/node/chrd/strg.h"

namespace shendk {

struct CHRT : File {

    CHRT() = default;

    CHRT(const std::string& filepath) {
        read(filepath);
    }

    ~CHRT() {}

    chrd::STRG strg;
    chrd::CHRS chrs;

protected:
    virtual void _read(std::istream& stream) {
        // skip chrs
        Node::Header chrsHeader;
        stream.read(reinterpret_cast<char*>(&chrsHeader), sizeof(Node::Header));
        stream.seekg(baseOffset + chrsHeader.size, std::ios::beg);

        // read strg
        strg.read(stream);

        // read chrs
        chrs.strg = &strg;
        stream.seekg(baseOffset, std::ios::beg);
        chrs.read(stream);
    }

    virtual void _write(std::ostream& stream) {
        throw new std::runtime_error("Not implemented");
    }

};

}
