#include "shendk/files/image/bmp.h"

#include "shendk/files/image/stb/stb_image_wrapper.h"

#include <iostream>

namespace shendk {

BMP::BMP() = default;
BMP::BMP(const std::string& filepath) { read(filepath); }
BMP::BMP(std::shared_ptr<Image> image) { mipmaps.push_back(image); }
BMP::~BMP() {}

void BMP::_read(std::istream& stream) {
    int width, height, bpp;
    //unsigned char* rgba = stbi_load("myimage.png", &width, &height, &bpp, 4);

    stream.seekg(0, std::ios::end);
    int64_t fileSize = stream.tellg();
    stbi_uc* buffer = new stbi_uc[fileSize];
    unsigned char* rgba = stbi_load_from_memory(buffer, fileSize, &width, &height, &bpp, 4);

    for (int i = 0; i < width*height*4; i++) {
        std::cout << rgba[i] << std::endl;
    }

    stbi_image_free(rgba);
    delete[] buffer;
}

void BMP::_write(std::ostream& stream) {

}

bool BMP::_isValid(uint32_t signature) {
    return false;
}

}
