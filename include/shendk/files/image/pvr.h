#pragma once

#include "shendk/files/file.h"

namespace shendk {

struct PVR : File {

    PVR() = default;

    PVR(const std::string& filepath) {
        read(filepath);
    }

    ~PVR() {}

protected:
    virtual void _read(std::istream& stream) {

    }

    virtual void _write(std::ostream& stream) {

    }

};

}
