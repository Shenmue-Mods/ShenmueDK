#pragma once

#include "shendk/files/file.h"

namespace shendk {

struct MAPINFO : File {

    MAPINFO() = default;

    MAPINFO(const std::string& filepath) {
        read(filepath);
    }

    ~MAPINFO() {}

protected:
    virtual void _read(std::istream& stream) {

    }

    virtual void _write(std::ostream& stream) {

    }

};

}
