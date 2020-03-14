#include "shendk/files/image/png.h"

#include "shendk/files/image/stb/stb_image_wrapper.h"
#include "shendk/files/image/stb/stb_image_write_wrapper.h"

#include <cstring>
#include <iostream>

namespace shendk {

PNG::PNG() = default;
PNG::PNG(const std::string& filepath) { read(filepath); }
PNG::PNG(std::istream& stream) { read(stream); }
PNG::PNG(std::shared_ptr<Image> image) { mipmaps.push_back(image); }
PNG::~PNG() {}

void PNG::_read(std::istream& stream) {
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

void PNG::_write(std::ostream& stream) {
    std::shared_ptr<Image> img = getImage();
    stbi_write_png_to_func(writeStbToStream, &stream, img->width(), img->height(), 4, &img->operator[](0), img->width() * 4);
}

bool PNG::_isValid(uint32_t signature) {
    return false;
}

}
