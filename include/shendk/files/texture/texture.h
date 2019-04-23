#pragma once
#pragma once

#include "shendk/files/file.h"
#include "shendk/types/image.h"

namespace shendk {

struct Texture : File {

    std::vector<ArgbImage> mipmaps;

};

}

