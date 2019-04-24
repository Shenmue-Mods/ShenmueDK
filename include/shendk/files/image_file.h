#pragma once
#pragma once

#include <memory>

#include "shendk/files/file.h"
#include "shendk/types/image.h"
#include "shendk/types/texture_id.h"

namespace shendk {

struct ImageFile : File {

    std::shared_ptr<Image> getImage(uint8_t mipmap = 0) {
        if (mipmap >= mipmaps.size()) {
            mipmap = mipmaps.size() - 1;
        }
        return mipmaps[mipmap];
    }

    std::vector<std::shared_ptr<Image>> mipmaps;

};

}

