#pragma once

#include "shendk/files/model_file.h"

namespace shendk {

/**
 * @brief COLLADA 1.4 model file.
 */
struct DAE : public ModelFile {

    DAE();
    DAE(Model m);
    DAE(const std::string& filepath);
    ~DAE();

protected:
    virtual void _read(std::istream& stream);
    virtual void _write(std::ostream& stream);
    virtual bool _isValid(uint32_t signature);

};

}
