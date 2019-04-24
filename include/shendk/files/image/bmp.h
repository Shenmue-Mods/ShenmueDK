#pragma once

#include "shendk/files/image_file.h"

namespace shendk {

struct BMP : ImageFile {

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
