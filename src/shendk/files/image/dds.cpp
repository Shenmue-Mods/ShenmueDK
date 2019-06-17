#include "shendk/files/image/dds.h"

#include "IL/il.h"
#include "IL/ilu.h"

namespace shendk {

DDS::DDS() = default;
DDS::DDS(const std::string& filepath) { read(filepath); }
DDS::DDS(std::istream& stream) { read(stream); }
DDS::DDS(std::shared_ptr<Image> image, DXTC mode) {
    mipmaps.push_back(image);
    dxtc = mode;
}
DDS::~DDS() {}

void DDS::_read(std::istream& stream) {

    stream.seekg(0, std::ios::end);
    int64_t size = stream.tellg() - baseOffset;
    stream.seekg(baseOffset, std::ios::beg);

    ILubyte* buffer = new ILubyte[size];
    stream.read(reinterpret_cast<char*>(buffer), size);

    ilInit();
    ILuint imgId = ilGenImage();
    ilBindImage(imgId);
    ilLoadL(IL_DDS, buffer, size);

    ILint channels, width, height;
    channels = ilGetInteger(IL_IMAGE_CHANNELS);
    width = ilGetInteger(IL_IMAGE_WIDTH);
    height = ilGetInteger(IL_IMAGE_HEIGHT);

    Image img(width, height);
    ILubyte * bytes = ilGetData();
    if (channels == 4) {
        int x, y;
        for (y = 0; y < height; y++) {
            for (x = 0; x < width; x++) {
                img[y * width + x].r = bytes[(y*width + x)*4 + 0];
                img[y * width + x].g = bytes[(y*width + x)*4 + 1];
                img[y * width + x].b = bytes[(y*width + x)*4 + 2];
                img[y * width + x].a = bytes[(y*width + x)*4 + 3];
            }
        }
    } else if (channels == 3) {
        int x, y;
        for (y = 0; y < height; y++) {
            for (x = 0; x < width; x++) {
                img[y * width + x].r = bytes[(y*width + x)*3 + 0];
                img[y * width + x].g = bytes[(y*width + x)*3 + 1];
                img[y * width + x].b = bytes[(y*width + x)*3 + 2];
                img[y * width + x].a = 255;
            }
        }
    } else {
        throw std::runtime_error("DDS: Invalid number of channels!");
    }
    mipmaps.push_back(std::shared_ptr<Image>(new Image(img)));

    delete[] buffer;
    ilDeleteImage(imgId);
}

void DDS::_write(std::ostream& stream) {

    std::shared_ptr<Image> img = mipmaps.front();
    ILint channels, width, height;
    width = img->width();
    height = img->height();
    channels = 4;

    ilInit();
    ILuint imgId = ilGenImage();
    ilBindImage(imgId);

    ILubyte* bytes = new ILubyte[width * height * channels];
    RGBA* pixels = img->getDataPtr();
    int x, y;
    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            bytes[(y*width + x) * 4 + 0] = pixels[y*width + x].r;
            bytes[(y*width + x) * 4 + 1] = pixels[y*width + x].g;
            bytes[(y*width + x) * 4 + 2] = pixels[y*width + x].b;
            bytes[(y*width + x) * 4 + 3] = pixels[y*width + x].a;
        }
    }

    ilTexImage(width, height, 1, 4, IL_RGBA, IL_UNSIGNED_BYTE, bytes);
    iluBuildMipmaps();

    ILinfo imageInfo;
    iluGetImageInfo(&imageInfo);

    if (dxtc == DXTC::DXT1) {
        ilSetInteger(IL_DXTC_FORMAT, IL_DXT1);
    } else if (dxtc == DXTC::DXT3) {
        ilSetInteger(IL_DXTC_FORMAT, IL_DXT3);
    }

    char* buffer = new char[imageInfo.SizeOfData];
    ILuint size = ilSaveL(IL_DDS, buffer, imageInfo.SizeOfData);
    stream.write(buffer, size);
    delete[] buffer;

    delete[] bytes;
    ilDeleteImage(imgId);
}

bool DDS::_isValid(uint32_t signature) {
    return false;
}

}
