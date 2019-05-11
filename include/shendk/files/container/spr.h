#pragma once

#include <stdint.h>

#include "shendk/files/container_file.h"
#include "shendk/node/texn.h"

namespace shendk {

struct SPR : public ContainerFile {

    typedef TEXN Entry;

    SPR();
    SPR(std::istream& stream);
    SPR(const std::string& filepath);

    virtual void unpack(const std::string& folder);

    std::vector<TEXN> entries;

protected:
    virtual void _read(std::istream& stream);
    virtual void _write(std::ostream& stream);
    virtual bool _isValid(uint32_t signature);
};

}
