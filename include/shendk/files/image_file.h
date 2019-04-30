#pragma once

#include <memory>
#include <vector>

#include "shendk/files/file.h"
#include "shendk/types/image.h"

namespace shendk {

struct ImageFile : File {

    std::shared_ptr<Image> getImage(uint8_t mipmap = 0);
    std::vector<std::shared_ptr<Image>> mipmaps;

};

}

