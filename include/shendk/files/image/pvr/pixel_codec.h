#pragma once

#include <stdint.h>

#include "shendk/files/image/pvr/formats.h"

namespace shendk {
namespace pvr {

struct PixelCodec {
    virtual ~PixelCodec();
    virtual uint16_t bpp() = 0;
    virtual void decodePixel(uint8_t* src, uint64_t srcIndex, uint8_t* dst, uint64_t dstIndex) = 0;
    virtual void encodePixel(uint8_t* src, uint64_t srcIndex, uint8_t* dst, uint64_t dstIndex) = 0;

    uint8_t* decodePalette(uint8_t* src, uint64_t srcIndex, uint32_t numEntries);
    uint8_t* encodePalette(uint8_t* palette, uint32_t numEntries);

    static PixelCodec* getPixelCodec(PixelFormat format);
};

struct ARGB1555 : public PixelCodec {
    virtual uint16_t bpp();
    virtual void decodePixel(uint8_t* src, uint64_t srcIndex, uint8_t* dst, uint64_t dstIndex);
    virtual void encodePixel(uint8_t* src, uint64_t srcIndex, uint8_t* dst, uint64_t dstIndex);
};

struct RGB565 : public PixelCodec {
    virtual uint16_t bpp();
    virtual void decodePixel(uint8_t* src, uint64_t srcIndex, uint8_t* dst, uint64_t dstIndex);
    virtual void encodePixel(uint8_t* src, uint64_t srcIndex, uint8_t* dst, uint64_t dstIndex);
};

struct ARGB4444 : public PixelCodec {
    virtual uint16_t bpp();
    virtual void decodePixel(uint8_t* src, uint64_t srcIndex, uint8_t* dst, uint64_t dstIndex);
    virtual void encodePixel(uint8_t* src, uint64_t srcIndex, uint8_t* dst, uint64_t dstIndex);
};

struct YUV422 : public PixelCodec {
    virtual uint16_t bpp();
    virtual void decodePixel(uint8_t* src, uint64_t srcIndex, uint8_t* dst, uint64_t dstIndex);
    virtual void encodePixel(uint8_t* src, uint64_t srcIndex, uint8_t* dst, uint64_t dstIndex);
};

struct BUMP88 : public PixelCodec {
    virtual uint16_t bpp();
    virtual void decodePixel(uint8_t* src, uint64_t srcIndex, uint8_t* dst, uint64_t dstIndex);
    virtual void encodePixel(uint8_t* src, uint64_t srcIndex, uint8_t* dst, uint64_t dstIndex);
};

struct RGB555 : public PixelCodec {
    virtual uint16_t bpp();
    virtual void decodePixel(uint8_t* src, uint64_t srcIndex, uint8_t* dst, uint64_t dstIndex);
    virtual void encodePixel(uint8_t* src, uint64_t srcIndex, uint8_t* dst, uint64_t dstIndex);
};

struct ARGB8888 : public PixelCodec {
    virtual uint16_t bpp();
    virtual void decodePixel(uint8_t* src, uint64_t srcIndex, uint8_t* dst, uint64_t dstIndex);
    virtual void encodePixel(uint8_t* src, uint64_t srcIndex, uint8_t* dst, uint64_t dstIndex);
};

}
}
