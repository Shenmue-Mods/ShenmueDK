#pragma once

#include "shendk/files/image_file.h"

namespace shendk {

struct PNG : ImageFile {

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
