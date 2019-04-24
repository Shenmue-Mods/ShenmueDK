#pragma once

#include "shendk/files/image_file.h"

namespace shendk {

struct DDS : ImageFile {

    DDS() = default;

    DDS(const std::string& filepath) {
        read(filepath);
    }

    ~DDS() {}

protected:
    virtual void _read(std::istream& stream) {
        int64_t baseOffset = stream.tellg();
        stream.seekg(0, std::ios::end);
        uint32_t size = static_cast<uint32_t>(stream.tellg() - baseOffset);

        char* buffer = new char[size];

    }

    virtual void _write(std::ostream& stream) {

    }

};

}
