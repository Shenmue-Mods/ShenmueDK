#pragma once

#include <vector>

#include "shendk/files/file.h"
#include "shendk/types/texture.h"

namespace shendk {
namespace obj {

struct MTL : public File {
    MTL();
    MTL(const std::string& filepath);

    std::vector<Texture> textures;
    std::vector<std::string> materialNames;

protected:
    virtual void _read(std::istream& stream);
    virtual void _write(std::ostream& stream);
    virtual bool _isValid(uint32_t signature);
};

}
}


