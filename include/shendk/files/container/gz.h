#pragma once

#include "shendk/files/file.h"

namespace shendk {

struct GZ : File {

    GZ();
    GZ(const std::string& filepath);
    GZ(std::istream& stream);
    ~GZ();

    static bool testGzip(std::istream& stream);
    static char* inflateStream(std::istream& inStream, uint64_t& bufferSize);

protected:
    virtual void _read(std::istream& stream);
    virtual void _write(std::ostream& stream);
    virtual bool _isValid(uint32_t signature);
};
}



