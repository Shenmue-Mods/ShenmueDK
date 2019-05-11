#pragma once

#include "shendk/files/file.h"
#include "shendk/files/container/ipac.h"

namespace shendk {

struct PKS : public File {
    const static uint32_t signature = 1397440848;

    struct Header {
        uint32_t signature;
        uint32_t ipacOffset;
        uint32_t unknown1;
        uint32_t unknown2;
    };

    PKS();
    PKS(std::istream& stream);
    PKS(const std::string& filepath);
    ~PKS();

    PKS::Header header;
    IPAC ipac;

protected:
    virtual void _read(std::istream& stream);
    virtual void _write(std::ostream& stream);
    virtual bool _isValid(uint32_t signature);
};

}
