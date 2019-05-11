#pragma once

#include "shendk/files/model_file.h"

namespace shendk {

struct OBJ : public ModelFile {

    OBJ();
    OBJ(Model m);
    OBJ(const std::string& filepath);

protected:
    virtual void _read(std::istream& stream);
    virtual void _write(std::ostream& stream);
    virtual bool _isValid(uint32_t signature);

};

}
