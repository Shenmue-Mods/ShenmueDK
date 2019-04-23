#pragma once
#pragma once

#include <memory>

#include "shendk/files/file.h"
#include "shendk/types/image.h"

namespace shendk {

struct Texture : File {

    std::vector<std::shared_ptr<Image>> mipmaps;

};

}

