#include "shendk/files/image/dds.h"

//include "nvimage/DirectDrawSurface.h"
//#include "nvcore/StdStream.h"

namespace shendk {

DDS::DDS() = default;
DDS::DDS(const std::string& filepath) { read(filepath); }
DDS::~DDS() {}

void DDS::_read(std::istream& stream) {
    int64_t baseOffset = stream.tellg();
    stream.seekg(0, std::ios::end);
    uint32_t size = static_cast<uint32_t>(stream.tellg() - baseOffset);
    /*uint8* buffer = new uint8[size];
    nv::MemoryInputStream memstream(buffer, size);
    nv::DirectDrawSurface dds(&memstream);

    for (int i = 0; i < dds.mipmapCount(); i++) {
        nv::Image img;
        dds.faceSize()
        dds.mipmap(&img, )
    }*/
}

void DDS::_write(std::ostream& stream) {

}

bool DDS::_isValid(uint32_t signature) {
    return false;
}

}
