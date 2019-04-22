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

    virtual bool canEncode() = 0;
    virtual uint16_t bpp() = 0;
    virtual uint16_t paletteEntries() { return 0; }
    virtual bool needsExternalPalette() { return false; }
    virtual bool hasMipmaps() { return false; }
    virtual bool vq() { return false; }

    uint8_t* decode(std::istream& stream, uint16_t width, uint16_t height, PixelCodec* codec) {
        pixelCodec = codec;
        double d = pixelCodec->bpp() / 8.0;
        uint64_t dataSize = width * height * d;
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

    void setPalette(std::istream& stream, uint32_t numEntries) {
        uint64_t bufferSize = bpp() * numEntries;
        uint8_t* buffer = new uint8_t[bufferSize];
        stream.read(reinterpret_cast<char*>(buffer), bufferSize);
        setPalette(buffer, 0, numEntries);
        delete[] buffer;
    }

    void setPalette(uint8_t* palette, uint64_t offset, uint32_t numEntries) {
        m_palette = pixelCodec->decodePalette(palette, offset, numEntries);
    }

    PixelCodec* pixelCodec;

protected:

    virtual uint8_t* decode(uint8_t* src, uint64_t srcIndex, uint16_t width, uint16_t height) = 0;
    virtual uint8_t* encode(uint8_t* src, uint64_t srcIndex, uint16_t width, uint16_t height) = 0;

    uint64_t* makeTwiddleMap(uint64_t size) {
        uint64_t* twiddleMap = new uint64_t[size];
        for (uint64_t i = 0; i < size; i++) {
            twiddleMap[i] = 0;
            for (uint64_t j = 0, k = 1; k <= i; j++, k <<= 1) {
                twiddleMap[i] |= (i & k) << j;
            }
        }
        return twiddleMap;
    }

    uint8_t* m_palette;

};


struct SquareTwiddled : public DataCodec {
    bool canEncode() { return true; }
    uint16_t bpp() { return pixelCodec->bpp(); }

    uint8_t* decode(uint8_t* src, uint64_t srcIndex, uint16_t width, uint16_t height) {
        uint8_t* destination = new uint8_t[width * height * 4];
        uint64_t destinationIndex = 0;
        uint64_t* twiddleMap = makeTwiddleMap(width);
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                pixelCodec->decodePixel(src, srcIndex + (((twiddleMap[x] << 1) | twiddleMap[y]) << (pixelCodec->bpp() >> 4)), destination, destinationIndex);
                destinationIndex += 4;
            }
        }
        delete[] twiddleMap;
        return destination;
    }

    uint8_t* encode(uint8_t* src, uint64_t srcIndex, uint16_t width, uint16_t height) {
        uint8_t* destination = new uint8_t[width * height * (pixelCodec->bpp() >> 3)];
        uint64_t* twiddleMap = makeTwiddleMap(width);
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                pixelCodec->encodePixel(src, srcIndex, destination, ((twiddleMap[x] << 1) | twiddleMap[y]) << (pixelCodec->bpp() >> 4));
                srcIndex += 4;
            }
        }
        return destination;
    }
};

struct SquareTwiddledMipmaps : public SquareTwiddled {
    bool hasMipmaps() { return true; }
};

struct VQ : public DataCodec {
    bool canEncode() { return true; }
    bool vq() { return true; }
    uint16_t bpp() { return 2; }
    uint16_t paletteEntries() { return 1024; } // 256 * 4 texels

    uint8_t* decode(uint8_t* src, uint64_t srcIndex, uint16_t width, uint16_t height) {
        uint8_t* destination = new uint8_t[width * height * 4];
        uint64_t destinationIndex;

        // 1x1 texture (no twiddle)
        if (width == 1 && height == 1) {
            int index = src[srcIndex] * 4;
            destinationIndex = 0;
            for (int i = 0; i < 4; i++) {
                destination[destinationIndex] = m_palette[index * 4 + i];
                destinationIndex++;
            }
            return destination;
        }

        uint64_t* twiddleMap = makeTwiddleMap(width);
        for (int y = 0; y < height; y += 2) {
            for (int x = 0; x < width; x += 2) {
                int index = src[srcIndex + ((twiddleMap[x >> 1] << 1) | twiddleMap[y >> 1])] * 4;
                for (int x2 = 0; x2 < 2; x2++) {
                    for (int y2 = 0; y2 < 2; y2++) {
                        destinationIndex = ((((y + y2) * width) + (x + x2)) * 4);
                        for (int i = 0; i < 4; i++) {
                            destination[destinationIndex] = m_palette[index * 4 + i];
                            destinationIndex++;
                        }
                        index++;
                    }
                }
            }
        }
        delete[] twiddleMap;
        return destination;
    }

    uint8_t* encode(uint8_t* src, uint64_t srcIndex, uint16_t width, uint16_t height) {
        uint16_t compressedWidth = width / 2;
        uint16_t compressedHeight = height / 2;
        uint8_t* destination = new uint8_t[compressedWidth * compressedHeight];
        uint64_t destinationIndex = 0;
        uint16_t size = std::min(compressedWidth, compressedHeight);
        uint64_t* twiddleMap = makeTwiddleMap(size);
        for (uint16_t y = 0; y < compressedHeight; y++) {
            for (uint16_t x = 0; x < compressedWidth; x++) {
                destinationIndex = (twiddleMap[x] << 1) | twiddleMap[y];
                destination[destinationIndex] = src[srcIndex];
                srcIndex++;
            }
        }
        delete[] twiddleMap;
        return destination;
    }
};

struct VQMipmaps : public VQ {
    bool hasMipmaps() { return true; }
};

struct Index4 : public DataCodec {
    bool canEncode() { return true; }
    uint16_t bpp() { return 4; }
    uint16_t paletteEntries() { return 16; }
    bool needsExternalPalette() { return true; }

    uint8_t* decode(uint8_t* src, uint64_t srcIndex, uint16_t width, uint16_t height) {
        uint8_t* destination = new uint8_t[width * height * 4];
        uint64_t destinationIndex;
        uint64_t size = std::min(width, height);
        uint64_t* twiddleMap = makeTwiddleMap(size);
        for (int y = 0; y < height; y += size) {
            for (int x = 0; x < width; x += size) {
                for (uint64_t y2 = 0; y2 < size; y2++) {
                    for (uint64_t x2 = 0; x2 < size; x2++) {
                        uint8_t index = ((src[srcIndex + (((twiddleMap[x2] << 1) | twiddleMap[y2]) >> 1)] >> ((y2 & 0x1) * 4)) & 0xF);
                        destinationIndex = ((((y + y2) * width) + (x + x2)) * 4);
                        for (int i = 0; i < 4; i++) {
                            destination[destinationIndex] = m_palette[index * 4 + i];
                            destinationIndex++;
                        }
                    }
                }
                srcIndex += (size * size) >> 1;
            }
        }
        delete[] twiddleMap;
        return destination;
    }

    uint8_t* encode(uint8_t* src, uint64_t srcIndex, uint16_t width, uint16_t height) {
        uint8_t* destination = new uint8_t[(width * height) >> 1];
        uint64_t destinationIndex = 0;
        uint64_t size = std::min(width, height);
        uint64_t* twiddleMap = makeTwiddleMap(size);
        for (int y = 0; y < height; y += size) {
            for (int x = 0; x < width; x += size) {
                for (uint64_t y2 = 0; y2 < size; y2++) {
                    for (uint64_t x2 = 0; x2 < size; x2++) {
                        uint8_t index = destination[destinationIndex + (((twiddleMap[x2] << 1) | twiddleMap[y2]) >> 1)];
                        index |= (src[srcIndex + (((y + y2) * width) + (x + x2))] & 0xF) << ((y2 & 0x1) * 4);
                        destination[destinationIndex + (((twiddleMap[x2] << 1) | twiddleMap[y2]) >> 1)] = index;
                    }
                }
                destinationIndex += (size * size) >> 1;
            }
        }
        delete[] twiddleMap;
        return destination;
    }
};

struct Index4Mipmap : public Index4 {
    bool hasMipmaps() { return true; }
};

struct Index8 : public DataCodec {
    bool canEncode() { return true; }
    uint16_t bpp() { return 8; }
    uint16_t paletteEntries() { return 256; }
    bool needsExternalPalette() { return true; }

    uint8_t* decode(uint8_t* src, uint64_t srcIndex, uint16_t width, uint16_t height) {
        uint8_t* destination = new uint8_t[width * height * 4];
        uint64_t destinationIndex;
        uint64_t size = std::min(width, height);
        uint64_t* twiddleMap = makeTwiddleMap(size);
        for (int y = 0; y < height; y += size) {
            for (int x = 0; x < width; x += size) {
                for (uint64_t y2 = 0; y2 < size; y2++) {
                    for (uint64_t x2 = 0; x2 < size; x2++) {
                        uint8_t index = src[srcIndex + ((twiddleMap[x2] << 1) | twiddleMap[y2])];
                        destinationIndex = ((((y + y2) * width) + (x + x2)) * 4);
                        for (int i = 0; i < 4; i++) {
                            destination[destinationIndex] = m_palette[index * 4 + i];
                            destinationIndex++;
                        }
                    }
                }
                srcIndex += (size * size);
            }
        }
        delete[] twiddleMap;
        return destination;
    }

    uint8_t* encode(uint8_t* src, uint64_t srcIndex, uint16_t width, uint16_t height) {
        uint8_t* destination = new uint8_t[width * height];
        uint64_t destinationIndex = 0;
        uint64_t size = std::min(width, height);
        uint64_t* twiddleMap = makeTwiddleMap(size);
        for (int x = 0; x < width; x += size) {
            for (int y = 0; y < height; y += size) {
                for (uint64_t y2 = 0; y2 < size; y2++) {
                    for (uint64_t x2 = 0; x2 < size; x2++) {
                        destination[destinationIndex + ((twiddleMap[x2] << 1) | twiddleMap[y2])] = src[srcIndex + (((y + y2) * width) + (x + x2))];
                    }
                }
                destinationIndex += (size * size);
            }
        }
        return destination;
    }
};

struct Index8Mipmap : public Index8 {
    bool hasMipmaps() { return true; }
};

struct Rectangle : public DataCodec {
    bool canEncode() { return true; }
    uint16_t bpp() { return pixelCodec->bpp(); }

    uint8_t* decode(uint8_t* src, uint64_t srcIndex, uint16_t width, uint16_t height) {
        uint8_t* destination = new uint8_t[width * height * 4];
        uint64_t destinationIndex = 0;
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                pixelCodec->decodePixel(src, srcIndex, destination, destinationIndex);
                srcIndex += (pixelCodec->bpp() >> 3);
                destinationIndex += 4;
            }
        }
        return destination;
    }

    uint8_t* encode(uint8_t* src, uint64_t srcIndex, uint16_t width, uint16_t height) {
        uint8_t* destination = new uint8_t[width * height * (pixelCodec->bpp() >> 3)];
        uint64_t destinationIndex = 0;
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                pixelCodec->encodePixel(src, srcIndex, destination, destinationIndex);
                srcIndex += 4;
                destinationIndex += (pixelCodec->bpp() >> 3);
            }
        }
        return destination;
    }
};

struct RectangleTwiddled : public DataCodec {
    bool canEncode() { return true; }
    uint16_t bpp() { return pixelCodec->bpp(); }

    uint8_t* decode(uint8_t* src, uint64_t srcIndex, uint16_t width, uint16_t height) {
        uint8_t* destination = new uint8_t[width * height * 4];
        uint16_t size = std::min(width, height);
        uint64_t* twiddleMap = makeTwiddleMap(size);
        for (int y = 0; y < height; y += size) {
            for (int x = 0; x < width; x += size) {
                for (int y2 = 0; y2 < size; y2++) {
                    for (int x2 = 0; x2 < size; x2++) {
                        pixelCodec->decodePixel(src, srcIndex + (((twiddleMap[x2] << 1) | twiddleMap[y2]) << (pixelCodec->bpp() >> 4)), destination, ((((y + y2) * width) + (x + x2)) * 4));
                    }
                }
                srcIndex += (size * size) * (pixelCodec->bpp() >> 3);
            }
        }
        delete[] twiddleMap;
        return destination;
    }

    uint8_t* encode(uint8_t* src, uint64_t srcIndex, uint16_t width, uint16_t height) {
        uint8_t*  destination = new uint8_t[width * height * (pixelCodec->bpp() >> 3)];
        uint64_t destinationIndex = 0;
        uint16_t size = std::min(width, height);
        uint64_t* twiddleMap = makeTwiddleMap(size);
        for (int y = 0; y < height; y += size) {
            for (int x = 0; x < width; x += size) {
                for (int y2 = 0; y2 < size; y2++) {
                    for (int x2 = 0; x2 < size; x2++) {
                        pixelCodec->encodePixel(src, srcIndex + ((((y + y2) * width) + (x + x2)) * 4), destination, destinationIndex + (((twiddleMap[x2] << 1) | twiddleMap[y2]) << (pixelCodec->bpp() >> 4)));
                    }
                }
                destinationIndex += (size * size) * (pixelCodec->bpp() >> 3);
            }
        }
        delete[] twiddleMap;
        return destination;
    }
};


static DataCodec* getDataCodec(PvrDataFormat format)
{
    switch (format)
    {
        case PvrDataFormat::SQUARE_TWIDDLED:
            return new SquareTwiddled();
        case PvrDataFormat::SQUARE_TWIDDLED_MIPMAP:
        case PvrDataFormat::SQUARE_TWIDDLED_MIPMAP_ALT:
            return new SquareTwiddledMipmaps();
        case PvrDataFormat::VECTOR_QUANTIZATION:
        case PvrDataFormat::VECTOR_QUANTIZATION_SMALL:
            return new VQ();
        case PvrDataFormat::VECTOR_QUANTIZATION_MIPMAP:
        case PvrDataFormat::VECTOR_QUANTIZATION_SMALL_MIPMAP:
            return new VQMipmaps();
        case PvrDataFormat::PALETTIZE_4BIT:
            return new Index4();
        case PvrDataFormat::PALETTIZE_4BIT_MIPMAP:
            return new Index4Mipmap();
        case PvrDataFormat::PALETTIZE_8BIT:
            return new Index8();
        case PvrDataFormat::PALETTIZE_8BIT_MIPMAP:
            return new Index8Mipmap();
        case PvrDataFormat::RECTANGLE:
        case PvrDataFormat::RECTANGLE_STRIDE:
            return new Rectangle();
        case PvrDataFormat::RECTANGLE_TWIDDLED:
            return new RectangleTwiddled();
        default:
            return nullptr;
    }
}

}
}
