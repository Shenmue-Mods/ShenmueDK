#include "shendk/files/image_file.h"

namespace shendk {

std::shared_ptr<Image> ImageFile::getImage(uint8_t mipmap) {
    if (mipmap >= mipmaps.size()) {
        mipmap = mipmaps.size() - 1;
    }
    return mipmaps[mipmap];
}

}

