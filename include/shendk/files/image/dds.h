#pragma once

#include "shendk/files/file.h"

namespace shendk {

struct DDS : File {

    DDS() = default;

    DDS(const std::string& filepath) {
        read(filepath);
    }

    ~DDS() {}

protected:
    virtual void _read(std::ifstream& stream) {
        int64_t baseOffset = stream.tellg();
        stream.seekg(0, std::ios::end);
        uint32_t size = static_cast<uint32_t>(stream.tellg() - baseOffset);

        char* buffer = new char[size];

    }

    virtual void _write(std::ofstream& stream) {

    }

};

}
