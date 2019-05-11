#pragma once

#include "shendk/files/file.h"

namespace shendk {

struct MAPINFO : public File {

    MAPINFO();
    MAPINFO(const std::string& filepath);
    ~MAPINFO();

protected:
    virtual void _read(std::istream& stream);
    virtual void _write(std::ostream& stream);
    virtual bool _isValid(uint32_t signature);
};

}
