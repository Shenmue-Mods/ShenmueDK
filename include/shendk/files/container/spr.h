#pragma once

#include <stdint.h>

#include "shendk/files/container_file.h"
#include "shendk/node/texn.h"

namespace shendk {

struct SPR : ContainerFile {

    typedef TEXN Entry;

    std::vector<TEXN> entries;

protected:

    virtual void _read(std::istream& stream) {
        while(!stream.eof()) {
            TEXN entry;
            entry.read(stream);
            entries.push_back(entry);
        }
    }

    virtual void _write(std::ostream& stream) {
        for (auto& texn : entries) {
            texn.write(stream);
        }
    }

    virtual bool _isValid(unsigned int signature)
    {
        return false;
    }
};

}
