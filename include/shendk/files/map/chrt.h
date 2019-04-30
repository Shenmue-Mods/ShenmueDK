#pragma once

#include "shendk/files/file.h"
#include "shendk/node/chrd/chrs.h"
#include "shendk/node/chrd/strg.h"

namespace shendk {

struct CHRT : File {

    CHRT();
    CHRT(const std::string& filepath);
    ~CHRT();

    chrd::STRG strg;
    chrd::CHRS chrs;

protected:
    virtual void _read(std::istream& stream);
    virtual void _write(std::ostream& stream);
    virtual bool _isValid(uint32_t signature);
};

}
