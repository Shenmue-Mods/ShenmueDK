#pragma once

#include "shendk/files/image_file.h"
#include "shendk/files/image/pvr/formats.h"

namespace shendk {

struct PVR : public ImageFile {

    const static uint32_t gbix = 0x58494247;
    const static uint32_t pvrt = 0x54525650;

    struct GBIX {
        uint32_t signature = gbix;
        uint32_t size = 0;
        uint32_t index = 0;
    };

    struct Header {
        uint32_t signature = pvrt;
        uint32_t size;
        pvr::PixelFormat pixelFormat;
        pvr::DataFormat dataFormat;
        uint16_t pad10 = 0;
        uint16_t width;
        uint16_t height;
    };

    PVR();
    PVR(const std::string& filepath);
    PVR(std::istream& stream);
    ~PVR();

    PVR::Header header;
    PVR::GBIX globalIndex;

protected:
    virtual void _read(std::istream& stream);
    virtual void _write(std::ostream& stream);
    virtual bool _isValid(uint32_t signature);
};

}
