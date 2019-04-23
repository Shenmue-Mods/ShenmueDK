#pragma once

#include "shendk/files/texture/texture.h"

namespace shendk {

struct PNG : Texture {

    PNG() = default;

    PNG(const std::string& filepath) {
        read(filepath);
    }

    ~PNG() {}

protected:
    virtual void _read(std::istream& stream) {

    }

    virtual void _write(std::ostream& stream) {

    }

};

}
