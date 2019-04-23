#pragma once

#include "shendk/files/texture/texture.h"

namespace shendk {

struct BMP : Texture {

    BMP() = default;

    BMP(const std::string& filepath) {
        read(filepath);
    }

    ~BMP() {}

protected:
    virtual void _read(std::istream& stream) {

    }

    virtual void _write(std::ostream& stream) {

    }

};

}
