#include "shendk/files/image/bmp.h"

#include "shendk/files/image/stb/stb_image_wrapper.h"
#include "shendk/files/image/stb/stb_image_write_wrapper.h"

#include <cstring>
#include <iostream>

namespace shendk {

BMP::BMP() = default;
BMP::BMP(const std::string& filepath) { read(filepath); }
BMP::BMP(std::istream& stream) { read(stream); }
BMP::BMP(std::shared_ptr<Image> image) { mipmaps.push_back(image); }
BMP::~BMP() {}

void BMP::_read(std::istream& stream) {
    stream.seekg(0, std::ios::end);
    int64_t fileSize = stream.tellg();
    stbi_uc* buffer = new stbi_uc[fileSize];
    stream.seekg(0, std::ios::beg);
    stream.read(reinterpret_cast<char*>(buffer), fileSize);

    int width, height, bpp;
    unsigned char* rgba = stbi_load_from_memory(buffer, fileSize, &width, &height, &bpp, 4);

    std::shared_ptr<Image> image(new Image(width, height));
    memcpy(image->getDataPtr(), rgba, width * height * 4);
    mipmaps.push_back(image);

    stbi_image_free(rgba);
    delete[] buffer;
}

void BMP::_write(std::ostream& stream) {
    std::shared_ptr<Image> img = getImage();
    stbi_write_bmp_to_func(writeStbToStream, &stream, img->width(), img->height(), 4, &img->operator[](0));
}

bool BMP::_isValid(uint32_t signature) {
    return false;
}

}
