#include "shendk/files/image/dds.h"

#include "shendk/files/image/dds/nv_dds.h"

namespace shendk {

DDS::DDS() = default;
DDS::DDS(const std::string& filepath) { read(filepath); }
DDS::~DDS() {}

void DDS::_read(std::istream& stream) {
    /*nv_dds::CDDSImage image;
    image.load(stream);
    if (image.get_num_mipmaps() == 0) {
        nv_dds::CSurface surface = image.get_surface(0);
        std::shared_ptr<Image> img(new Image(surface.get_width(), surface.get_height()));
        memcpy(img->getDataPtr(), reinterpret_cast<void*>(surface.get_pixels()), img->size());
        mipmaps.push_back(img);
    } else {
        for (int i = 0; i < image.get_num_mipmaps(); i++) {
            nv_dds::CSurface surface = image.get_mipmap(i);
            std::shared_ptr<Image> img(new Image(surface.get_width(), surface.get_height()));
            memcpy(img->getDataPtr(), reinterpret_cast<void*>(*surface), surface.get_size());
            mipmaps.push_back(img);
        }
    }*/
}

void DDS::_write(std::ostream& stream) {
    /*nv_dds::CDDSImage image;
    image.save(stream);*/
}

bool DDS::_isValid(uint32_t signature) {
    return false;
}

}
