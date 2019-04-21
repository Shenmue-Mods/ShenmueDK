#pragma once

#include <stdint.h>
#include <cstring>
#include <algorithm>
#include <iostream>

#include "shendk/utils/math.h"
#include "shendk/files/image/pvr/pixel_codec.h"

namespace shendk {
namespace pvr {

struct DataCodec {

    virtual ~DataCodec() {
        delete[] m_palette;
    }

    virtual bool vq() = 0;
    virtual bool canEncode() = 0;
    virtual uint16_t bpp() = 0;
    virtual uint16_t paletteEntries() { return 0; }
    virtual bool needsExternalPalette() { return false; }
    virtual bool hasMipmaps() { return false; }
    virtual uint8_t* decode(uint8_t* src, uint64_t srcIndex, uint16_t width, uint16_t height) = 0;
    virtual uint8_t* encode(uint8_t* src, uint64_t srcIndex, uint16_t width, uint16_t height) = 0;


    uint8_t* decode(std::istream& stream, uint16_t width, uint16_t height, PixelCodec* codec) {
        pixelCodec = codec;
        double d = pixelCodec->bpp() / 8.0;
        size_t dataSize = width * height * d;
        uint8_t* data = new uint8_t[dataSize];
        stream.read(reinterpret_cast<char*>(data), dataSize);
        return decode(data, 0, width, height);
    }

    uint8_t* decode(uint8_t* src, uint16_t width, uint16_t height, PixelCodec* codec) {
        pixelCodec = codec;
        return encode(src, 0, width, height);
    }

    uint8_t* encode(uint8_t* src, uint16_t width, uint16_t height, PixelCodec* codec) {
        pixelCodec = codec;
        return encode(src, 0, width, height);
    }

    void setPalette(std::istream& stream, int numEntries) {
        size_t bufferSize = bpp() * numEntries;
        uint8_t* buffer = new uint8_t[bufferSize];
        stream.read(reinterpret_cast<char*>(buffer), bufferSize);
        setPalette(buffer, 0, numEntries);
        delete[] buffer;
    }

    void setPalette(uint8_t* palette, uint64_t offset, uint32_t numEntries) {
        m_palette = pixelCodec->decodePalette(palette, offset, numEntries);
    }




    PixelCodec* pixelCodec;

private:
    uint8_t* m_palette;

};



}
}
