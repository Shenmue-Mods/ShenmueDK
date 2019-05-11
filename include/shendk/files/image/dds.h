#pragma once

#include "shendk/files/image_file.h"

namespace shendk {

struct DDS : public ImageFile {

    DDS();
    DDS(const std::string& filepath);
    ~DDS();

protected:
    virtual void _read(std::istream& stream);
    virtual void _write(std::ostream& stream);
    virtual bool _isValid(uint32_t signature);
};

}
