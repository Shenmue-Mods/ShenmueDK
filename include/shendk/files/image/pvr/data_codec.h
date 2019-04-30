#pragma once

#include <stdint.h>
#include <iostream>

#include "shendk/files/image/pvr/pixel_codec.h"

namespace shendk {
namespace pvr {

struct DataCodec {

    virtual ~DataCodec();
    virtual uint16_t bpp() = 0;
    virtual uint16_t paletteEntries(uint16_t);
    virtual bool needsExternalPalette();
    virtual bool hasMipmaps();
    virtual bool vq();

    uint8_t* decode(std::istream& stream, uint16_t width, uint16_t height, PixelCodec* codec);
    uint8_t* decode(uint8_t* src, uint16_t width, uint16_t height, PixelCodec* codec);
    uint8_t* encode(uint8_t* src, uint16_t width, uint16_t height, PixelCodec* codec);

    void setPalette(std::istream& stream, uint32_t numEntries);
    void setPalette(uint8_t* palette, uint64_t offset, uint32_t numEntries);

    static DataCodec* getDataCodec(DataFormat format);

    PixelCodec* pixelCodec;

protected:
    virtual uint8_t* decode(uint8_t* src, uint64_t srcIndex, uint16_t width, uint16_t height) = 0;
    virtual uint8_t* encode(uint8_t* src, uint64_t srcIndex, uint16_t width, uint16_t height) = 0;

    uint8_t* m_palette;
};


struct SquareTwiddled : public DataCodec {
    virtual uint16_t bpp();
    virtual uint8_t* decode(uint8_t* src, uint64_t srcIndex, uint16_t width, uint16_t height);
    virtual uint8_t* encode(uint8_t* src, uint64_t srcIndex, uint16_t width, uint16_t height);
};

struct SquareTwiddledMipmaps : public SquareTwiddled {
    virtual bool hasMipmaps();
};

struct VQ : public DataCodec {
    virtual bool vq();
    virtual uint16_t bpp();
    virtual uint16_t paletteEntries(uint16_t);
    virtual uint8_t* decode(uint8_t* src, uint64_t srcIndex, uint16_t width, uint16_t height);
    virtual uint8_t* encode(uint8_t* src, uint64_t srcIndex, uint16_t width, uint16_t height);
};

struct VQMipmaps : public VQ {
    virtual bool hasMipmaps();
};

struct VQSmall : public VQ {
    virtual uint16_t paletteEntries(uint16_t width);
};

struct VQSmallMipmaps : public VQSmall {
    virtual bool hasMipmaps();
};

struct Index4 : public DataCodec {
    virtual uint16_t bpp();
    virtual uint16_t paletteEntries(uint16_t);
    virtual bool needsExternalPalette();
    virtual uint8_t* decode(uint8_t* src, uint64_t srcIndex, uint16_t width, uint16_t height);
    virtual uint8_t* encode(uint8_t* src, uint64_t srcIndex, uint16_t width, uint16_t height);
};

struct Index4Mipmap : public Index4 {
    virtual bool hasMipmaps();
};

struct Index8 : public DataCodec {
    virtual uint16_t bpp();
    virtual uint16_t paletteEntries(uint16_t);
    virtual bool needsExternalPalette();
    virtual uint8_t* decode(uint8_t* src, uint64_t srcIndex, uint16_t width, uint16_t height);
    virtual uint8_t* encode(uint8_t* src, uint64_t srcIndex, uint16_t width, uint16_t height);
};

struct Index8Mipmap : public Index8 {
    virtual bool hasMipmaps();
};

struct Rectangle : public DataCodec {
    virtual uint16_t bpp();
    uint8_t* decode(uint8_t* src, uint64_t srcIndex, uint16_t width, uint16_t height);
    uint8_t* encode(uint8_t* src, uint64_t srcIndex, uint16_t width, uint16_t height);
};

struct RectangleTwiddled : public DataCodec {
    bool canEncode();
    uint16_t bpp();
    uint8_t* decode(uint8_t* src, uint64_t srcIndex, uint16_t width, uint16_t height);
    uint8_t* encode(uint8_t* src, uint64_t srcIndex, uint16_t width, uint16_t height);
};

}
}
