#pragma once

#include <stdint.h>
#include <cstring>
#include <algorithm>

#include "shendk/utils/math.h"
#include "shendk/files/image/pvr/formats.h"

namespace shendk {
namespace pvr {

struct PixelCodec {

    virtual bool canEncode() = 0;
    virtual uint16_t bpp() = 0;

    virtual void decodePixel(uint8_t* src, uint64_t srcIndex, uint8_t* dst, uint64_t dstIndex) = 0;
    virtual void encodePixel(uint8_t* src, uint64_t srcIndex, uint8_t* dst, uint64_t dstIndex) = 0;

    uint8_t* decodePalette(uint8_t* src, uint64_t srcIndex, uint32_t numEntries) {
        // TODO: implement
        return nullptr;
    }

    uint8_t* encodePalette(uint8_t* palette, uint32_t numEntries) {
        // TODO: implement
        return nullptr;
    }
};

struct ARGB1555 : public PixelCodec {

    bool canEncode() { return true; }
    uint16_t bpp() { return 16; }

    void decodePixel(uint8_t* src, uint64_t srcIndex, uint8_t* dst, uint64_t dstIndex) {
        uint16_t pixel = *reinterpret_cast<uint16_t*>(src + srcIndex);
        dst[dstIndex + 3] = static_cast<uint8_t>(((pixel >> 15) & 0x01) * 0xFF);
        dst[dstIndex + 2] = static_cast<uint8_t>(((pixel >> 10) & 0x1F) * 0xFF / 0x1F);
        dst[dstIndex + 1] = static_cast<uint8_t>(((pixel >> 5) & 0x1F) * 0xFF / 0x1F);
        dst[dstIndex + 0] = static_cast<uint8_t>(((pixel >> 0) & 0x1F) * 0xFF / 0x1F);
    }

    void encodePixel(uint8_t* src, uint64_t srcIndex, uint8_t* dst, uint64_t dstIndex) {
        uint16_t pixel = 0x0000;
        pixel |= static_cast<uint16_t>((src[srcIndex + 3] >> 7) << 15);
        pixel |= static_cast<uint16_t>((src[srcIndex + 2] >> 3) << 10);
        pixel |= static_cast<uint16_t>((src[srcIndex + 1] >> 3) << 5);
        pixel |= static_cast<uint16_t>((src[srcIndex + 0] >> 3) << 0);
        dst[dstIndex + 1] = static_cast<uint8_t>((pixel >> 8) & 0xFF);
        dst[dstIndex + 0] = static_cast<uint8_t>(pixel & 0xFF);
    }
};

struct RGB565 : public PixelCodec {
    bool canEncode() { return true; }
    uint16_t bpp() { return 16; }

    void decodePixel(uint8_t* src, uint64_t srcIndex, uint8_t* dst, uint64_t dstIndex) {
        uint16_t pixel = *reinterpret_cast<uint16_t*>(src + srcIndex);
        dst[dstIndex + 3] = 0xFF;
        dst[dstIndex + 2] = static_cast<uint8_t>(((pixel >> 11) & 0x1F) * 0xFF / 0x1F);
        dst[dstIndex + 1] = static_cast<uint8_t>(((pixel >> 5)  & 0x3F) * 0xFF / 0x3F);
        dst[dstIndex + 0] = static_cast<uint8_t>(((pixel >> 0)  & 0x1F) * 0xFF / 0x1F);
    }

    void encodePixel(uint8_t* src, uint64_t srcIndex, uint8_t* dst, uint64_t dstIndex) {
        uint16_t pixel = 0x0000;
        pixel |= static_cast<uint16_t>((src[srcIndex + 2] >> 3) << 11);
        pixel |= static_cast<uint16_t>((src[srcIndex + 1] >> 2) << 5);
        pixel |= static_cast<uint16_t>((src[srcIndex + 0] >> 3) << 0);
        dst[dstIndex + 1] = static_cast<uint8_t>((pixel >> 8) & 0xFF);
        dst[dstIndex + 0] = static_cast<uint8_t>(pixel & 0xFF);
    }
};

struct ARGB4444 : public PixelCodec {
    bool canEncode() { return true; }
    uint16_t bpp() { return 16; }

    void decodePixel(uint8_t* src, uint64_t srcIndex, uint8_t* dst, uint64_t dstIndex) {
        uint16_t pixel = *reinterpret_cast<uint16_t*>(src + srcIndex);
        dst[dstIndex + 3] = static_cast<uint8_t>(((pixel >> 12) & 0x0F) * 0xFF / 0x0F);
        dst[dstIndex + 2] = static_cast<uint8_t>(((pixel >> 8)  & 0x0F) * 0xFF / 0x0F);
        dst[dstIndex + 1] = static_cast<uint8_t>(((pixel >> 4)  & 0x0F) * 0xFF / 0x0F);
        dst[dstIndex + 0] = static_cast<uint8_t>(((pixel >> 0)  & 0x0F) * 0xFF / 0x0F);
    }

    void encodePixel(uint8_t* src, uint64_t srcIndex, uint8_t* dst, uint64_t dstIndex) {
        uint16_t pixel = 0x0000;
        pixel |= static_cast<uint16_t>((src[srcIndex + 3] >> 4) << 12);
        pixel |= static_cast<uint16_t>((src[srcIndex + 2] >> 4) << 8);
        pixel |= static_cast<uint16_t>((src[srcIndex + 1] >> 4) << 4);
        pixel |= static_cast<uint16_t>((src[srcIndex + 0] >> 4) << 0);
        dst[dstIndex + 1] = static_cast<uint8_t>((pixel >> 8) & 0xFF);
        dst[dstIndex + 0] = static_cast<uint8_t>(pixel & 0xFF);
    }
};

struct YUV422 : public PixelCodec {
    bool canEncode() { return true; }
    uint16_t bpp() { return 32; } // using 32 Bpp because YUV requires two pixels to decode one pixel

    void decodePixel(uint8_t* src, uint64_t srcIndex, uint8_t* dst, uint64_t dstIndex) {
        uint16_t pixel1 = *reinterpret_cast<uint16_t*>(src + srcIndex);
        uint16_t pixel2 = *reinterpret_cast<uint16_t*>(src + srcIndex + 2);

        int Y0 = (pixel1 & 0xFF00) >> 8, U = (pixel1 & 0x00FF);
        int Y1 = (pixel2 & 0xFF00) >> 8, V = (pixel2 & 0x00FF);

        uint8_t r1 = std::clamp<uint8_t>(Y0 + 1.375 * (V - 128), 0, 255);
        uint8_t g1 = std::clamp<uint8_t>(Y0 - 0.6875 * (V - 128) - 0.34375 * (U - 128), 0, 255);
        uint8_t b1 = std::clamp<uint8_t>(Y0 + 1.71875 * (U - 128), 0, 255);

        uint8_t r2 = std::clamp<uint8_t>(Y1 + 1.375 * (V - 128), 0, 255);
        uint8_t g2 = std::clamp<uint8_t>(Y1 - 0.6875 * (V - 128) - 0.34375 * (U - 128), 0, 255);
        uint8_t b2 = std::clamp<uint8_t>(Y1 + 1.71875 * (U - 128), 0, 255);

        dst[dstIndex + 3] = 0xFF;
        dst[dstIndex + 2] = r1;
        dst[dstIndex + 1] = g1;
        dst[dstIndex + 0] = b1;

        dst[dstIndex + 4 + 3] = 0xFF;
        dst[dstIndex + 4 + 2] = r2;
        dst[dstIndex + 4 + 1] = g2;
        dst[dstIndex + 4 + 0] = b2;
    }

    void encodePixel(uint8_t* src, uint64_t srcIndex, uint8_t* dst, uint64_t dstIndex) {
        uint8_t r1 = src[srcIndex + 2];
        uint8_t g1 = src[srcIndex + 1];
        uint8_t b1 = src[srcIndex];

        uint8_t r2 = src[srcIndex + 4 + 2];
        uint8_t g2 = src[srcIndex + 4 + 1];
        uint8_t b2 = src[srcIndex + 4];

        uint32_t Y0 = static_cast<uint32_t>(0.299 * r1 + 0.587 * r2 + 0.114 * b1);
        uint32_t Y1 = static_cast<uint32_t>(0.299 * r2 + 0.587 * g2 + 0.114 * b2);

        uint8_t r = static_cast<uint8_t>((r2 + r1) / 2);
        uint8_t g = static_cast<uint8_t>((g2 + g1) / 2);
        uint8_t b = static_cast<uint8_t>((b2 + b1) / 2);

        uint32_t U = static_cast<uint32_t>(128.0f - 0.14f * r - 0.29f * g + 0.43f * b);
        uint32_t V = static_cast<uint32_t>(128.0f + 0.36f * r - 0.29f * g - 0.07f * b);

        uint16_t pixel1 = static_cast<uint16_t>((Y0 << 8) | U);
        uint16_t pixel2 = static_cast<uint16_t>((Y1 << 8) | V);

        dst[dstIndex + 3] = static_cast<uint8_t>((pixel2 >> 8) & 0xFF);
        dst[dstIndex + 2] = static_cast<uint8_t>(pixel2 & 0xFF);
        dst[dstIndex + 1] = static_cast<uint8_t>((pixel1 >> 8) & 0xFF);
        dst[dstIndex] = static_cast<uint8_t>(pixel1 & 0xFF);
    }
};

struct BUMP88 : public PixelCodec {
    bool canEncode() { return true; }
    uint16_t bpp() { return 16; }

    void decodePixel(uint8_t* src, uint64_t srcIndex, uint8_t* dst, uint64_t dstIndex) {
        uint8_t r = src[srcIndex];
        uint8_t s = src[srcIndex + 1];

        double rAngle = r / 255.0 * 360.0;
        double sAngle = s / 255.0 * 90.0;

        double rRadian = degreesToRadians(rAngle);
        double sRadian = degreesToRadians(sAngle);

        double x = std::cos(sRadian) * std::cos(rRadian);
        double y = std::cos(sRadian) * std::sin(rRadian);
        double z = std::sin(sRadian);

        double colorR = 0.5 * x + 0.5;
        double colorG = 0.5 * y + 0.5;
        double colorB = 0.5 * z + 0.5;

        dst[dstIndex + 3] = 0xFF;
        dst[dstIndex + 2] = static_cast<uint8_t>(colorR * 255.0);
        dst[dstIndex + 1] = static_cast<uint8_t>(colorG * 255.0);
        dst[dstIndex + 0] = static_cast<uint8_t>(colorB * 255.0);
    }

    void encodePixel(uint8_t* src, uint64_t srcIndex, uint8_t* dst, uint64_t dstIndex) {
        uint8_t colorR = src[srcIndex + 2];
        uint8_t colorG = src[srcIndex + 1];
        uint8_t colorB = src[srcIndex];

        double x = colorR / 255.0 * 2.0 - 1.0;
        double y = colorG / 255.0 * 2.0 - 1.0;
        double z = colorB / 255.0 * 2.0 - 1.0;

        double radius = std::sqrt(std::pow(x, 2) + std::pow(y, 2) + std::pow(z, 2));
        double rRadian = std::atan2(y, x);
        double sRadian = std::asin(z / radius);

        double rAngle = radiansToDegrees(rRadian);
        rAngle = rAngle < 0.0 ? rAngle + 360.0 : rAngle;
        double sAngle = radiansToDegrees(sRadian);

        rAngle = std::clamp<double>(rAngle, 0.0, 360.0);
        sAngle = std::clamp<double>(sAngle, 0.0, 90.0);

        uint8_t r = static_cast<uint8_t>(std::round(rAngle / 360.0 * 255.0));
        uint8_t s = static_cast<uint8_t>(std::round(sAngle / 90.0 * 255.0));

        dst[dstIndex + 1] = s;
        dst[dstIndex + 0] = r;
    }
};

struct RGB555 : public PixelCodec {
    bool canEncode() { return true; }
    uint16_t bpp() { return 16; }

    void decodePixel(uint8_t* src, uint64_t srcIndex, uint8_t* dst, uint64_t dstIndex) {
        uint16_t pixel = *reinterpret_cast<uint16_t*>(src + srcIndex);
        dst[dstIndex + 3] = 0xFF;
        dst[dstIndex + 2] = static_cast<uint8_t>(((pixel >> 10) & 0x1F) * 0xFF / 0x1F);
        dst[dstIndex + 1] = static_cast<uint8_t>(((pixel >> 5)  & 0x1F) * 0xFF / 0x1F);
        dst[dstIndex + 0] = static_cast<uint8_t>(((pixel >> 0)  & 0x1F) * 0xFF / 0x1F);
    }

    void encodePixel(uint8_t* src, uint64_t srcIndex, uint8_t* dst, uint64_t dstIndex) {
        uint16_t pixel = 0x0000;
        pixel |= static_cast<uint16_t>((src[srcIndex + 2] >> 3) << 10);
        pixel |= static_cast<uint16_t>((src[srcIndex + 1] >> 3) << 5);
        pixel |= static_cast<uint16_t>((src[srcIndex + 0] >> 3) << 0);
        dst[dstIndex + 1] = static_cast<uint8_t>((pixel >> 8) & 0xFF);
        dst[dstIndex + 0] = static_cast<uint8_t>(pixel & 0xFF);
    }
};

struct ARGB8888 : public PixelCodec {
    bool canEncode() { return true; }
    uint16_t bpp() { return 16; }

    void decodePixel(uint8_t* src, uint64_t srcIndex, uint8_t* dst, uint64_t dstIndex) {
        memcpy(dst + dstIndex, src + srcIndex, 4);
    }

    void encodePixel(uint8_t* src, uint64_t srcIndex, uint8_t* dst, uint64_t dstIndex) {
        memcpy(src + srcIndex, dst + dstIndex, 4);
    }
};

static PixelCodec* getPixelCodec(PvrPixelFormat format)
{
    switch (format)
    {
        case PvrPixelFormat::ARGB1555:
            return new ARGB1555();
        case PvrPixelFormat::RGB565:
            return new RGB565();
        case PvrPixelFormat::ARGB4444:
            return new ARGB4444();
        case PvrPixelFormat::YUV422:
            return new YUV422();
        case PvrPixelFormat::BUMP88:
            return new BUMP88();
        case PvrPixelFormat::RGB555:
            return new RGB555();
        case PvrPixelFormat::ARGB8888:
            return new ARGB8888();
        default:
            return nullptr;
    }
}

}
}
