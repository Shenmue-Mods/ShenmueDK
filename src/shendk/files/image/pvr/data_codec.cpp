#include "shendk/files/image/pvr/data_codec.h"

#include <cstring>
#include <algorithm>
#include "shendk/utils/math.h"
#include "shendk/files/image/pvr/twiddle.h"

namespace shendk {
namespace pvr {

DataCodec::~DataCodec() { delete[] m_palette; }

uint16_t DataCodec::paletteEntries(uint16_t) { return 0; }
bool DataCodec::needsExternalPalette() { return false; }
bool DataCodec::hasMipmaps() { return false; }
bool DataCodec::vq() { return false; }

uint8_t* DataCodec::decode(std::istream& stream, uint16_t width, uint16_t height, PixelCodec* codec) {
    pixelCodec = codec;
    double d = pixelCodec->bpp() / 8.0;
    uint64_t dataSize = width * height * d;
    uint8_t* data = new uint8_t[dataSize];
    stream.read(reinterpret_cast<char*>(data), dataSize);
    return decode(data, 0, width, height);
}

uint8_t* DataCodec::decode(uint8_t* src, uint16_t width, uint16_t height, PixelCodec* codec) {
    pixelCodec = codec;
    return encode(src, 0, width, height);
}

uint8_t* DataCodec::encode(uint8_t* src, uint16_t width, uint16_t height, PixelCodec* codec) {
    pixelCodec = codec;
    return encode(src, 0, width, height);
}

void DataCodec::setPalette(std::istream& stream, uint32_t numEntries) {
    uint64_t bufferSize = bpp() * numEntries;
    uint8_t* buffer = new uint8_t[bufferSize];
    stream.read(reinterpret_cast<char*>(buffer), bufferSize);
    setPalette(buffer, 0, numEntries);
    delete[] buffer;
}

void DataCodec::setPalette(uint8_t* palette, uint64_t offset, uint32_t numEntries) {
    m_palette = pixelCodec->decodePalette(palette, offset, numEntries);
}

DataCodec* DataCodec::getDataCodec(DataFormat format) {
    switch (format)
    {
    case DataFormat::SQUARE_TWIDDLED:
        return new SquareTwiddled();
    case DataFormat::SQUARE_TWIDDLED_MIPMAP:
    case DataFormat::SQUARE_TWIDDLED_MIPMAP_ALT:
        return new SquareTwiddledMipmaps();
    case DataFormat::VECTOR_QUANTIZATION:
        return new VQ();
    case DataFormat::VECTOR_QUANTIZATION_MIPMAP:
        return new VQMipmaps();
    case DataFormat::VECTOR_QUANTIZATION_SMALL:
        return new VQSmall();
    case DataFormat::VECTOR_QUANTIZATION_SMALL_MIPMAP:
        return new VQSmallMipmaps();
    case DataFormat::PALETTIZE_4BIT:
        return new Index4();
    case DataFormat::PALETTIZE_4BIT_MIPMAP:
        return new Index4Mipmap();
    case DataFormat::PALETTIZE_8BIT:
        return new Index8();
    case DataFormat::PALETTIZE_8BIT_MIPMAP:
        return new Index8Mipmap();
    case DataFormat::RECTANGLE:
    case DataFormat::RECTANGLE_STRIDE:
        return new Rectangle();
    case DataFormat::RECTANGLE_TWIDDLED:
        return new RectangleTwiddled();
    default:
        return nullptr;
    }
}


// SQUARE_TWIDDLED
uint16_t SquareTwiddled::bpp() { return pixelCodec->bpp(); }

uint8_t* SquareTwiddled::decode(uint8_t* src, uint64_t srcIndex, uint16_t width, uint16_t height) {
    uint8_t* destination = new uint8_t[width * height * 4];
    uint64_t destinationIndex = 0;
    uint64_t* twiddleMap = createTwiddleMap(width);
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            pixelCodec->decodePixel(src, srcIndex + (((twiddleMap[x] << 1) | twiddleMap[y]) << (pixelCodec->bpp() >> 4)), destination, destinationIndex);
            destinationIndex += 4;
        }
    }
    delete[] twiddleMap;
    return destination;
}

uint8_t* SquareTwiddled::encode(uint8_t* src, uint64_t srcIndex, uint16_t width, uint16_t height) {
    uint8_t* destination = new uint8_t[width * height * (pixelCodec->bpp() >> 3)];
    uint64_t* twiddleMap = createTwiddleMap(width);
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            pixelCodec->encodePixel(src, srcIndex, destination, ((twiddleMap[x] << 1) | twiddleMap[y]) << (pixelCodec->bpp() >> 4));
            srcIndex += 4;
        }
    }
    return destination;
}


// SQUARE_TWIDDLED_MIPMAP
bool SquareTwiddledMipmaps::hasMipmaps() { return true; }


// VECTOR_QUANTIZATION
bool VQ::vq() { return true; }
uint16_t VQ::bpp() { return 2; }
uint16_t VQ::paletteEntries(uint16_t) { return 1024; } // 256 * 4 texels

uint8_t* VQ::decode(uint8_t* src, uint64_t srcIndex, uint16_t width, uint16_t height) {
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

    uint64_t* twiddleMap = createTwiddleMap(width);
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

uint8_t* VQ::encode(uint8_t* src, uint64_t srcIndex, uint16_t width, uint16_t height) {
    uint16_t compressedWidth = width / 2;
    uint16_t compressedHeight = height / 2;
    uint8_t* destination = new uint8_t[compressedWidth * compressedHeight];
    uint64_t destinationIndex = 0;
    uint16_t size = std::min(compressedWidth, compressedHeight);
    uint64_t* twiddleMap = createTwiddleMap(size);
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


// VECTOR_QUANTIZATION_MIPMAP
bool VQMipmaps::hasMipmaps() { return true; }


// VECTOR_QUANTIZATION_SMALL
uint16_t VQSmall::paletteEntries(uint16_t width) {
    if      (width <= 16) { return 64; }
    else if (width <= 32) { return 256; }
    else if (width <= 64) { return 512; }
    else                  { return 1024; }
}


// VECTOR_QUANTIZATION_SMALL_MIPMAP
bool VQSmallMipmaps::hasMipmaps() { return true; }


// PALETTIZE_4BIT
bool canEncode() { return true; }
uint16_t Index4::bpp() { return 4; }
uint16_t Index4::paletteEntries(uint16_t) { return 16; }
bool Index4::needsExternalPalette() { return true; }

uint8_t* Index4::decode(uint8_t* src, uint64_t srcIndex, uint16_t width, uint16_t height) {
    uint8_t* destination = new uint8_t[width * height * 4];
    uint64_t destinationIndex;
    uint64_t size = std::min(width, height);
    uint64_t* twiddleMap = createTwiddleMap(size);
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

uint8_t* Index4::encode(uint8_t* src, uint64_t srcIndex, uint16_t width, uint16_t height) {
    uint8_t* destination = new uint8_t[(width * height) >> 1];
    uint64_t destinationIndex = 0;
    uint64_t size = std::min(width, height);
    uint64_t* twiddleMap = createTwiddleMap(size);
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


// PALETTIZE_4BIT_MIPMAP
bool Index4Mipmap::hasMipmaps() { return true; }


// PALETTIZE_8BIT
uint16_t Index8::bpp() { return 8; }
uint16_t Index8::paletteEntries(uint16_t) { return 256; }
bool Index8::needsExternalPalette() { return true; }

uint8_t* Index8::decode(uint8_t* src, uint64_t srcIndex, uint16_t width, uint16_t height) {
    uint8_t* destination = new uint8_t[width * height * 4];
    uint64_t destinationIndex;
    uint64_t size = std::min(width, height);
    uint64_t* twiddleMap = createTwiddleMap(size);
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

uint8_t* Index8::encode(uint8_t* src, uint64_t srcIndex, uint16_t width, uint16_t height) {
    uint8_t* destination = new uint8_t[width * height];
    uint64_t destinationIndex = 0;
    uint64_t size = std::min(width, height);
    uint64_t* twiddleMap = createTwiddleMap(size);
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


// PALETTIZE_8BIT_MIPMAP
bool Index8Mipmap::hasMipmaps() { return true; }


// RECTANGLE, RECTANGLE_STRIDE, RAW
uint16_t Rectangle::bpp() { return pixelCodec->bpp(); }

uint8_t* Rectangle::decode(uint8_t* src, uint64_t srcIndex, uint16_t width, uint16_t height) {
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

uint8_t* Rectangle::encode(uint8_t* src, uint64_t srcIndex, uint16_t width, uint16_t height) {
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


// RECTANGLE_TWIDDLED
uint16_t RectangleTwiddled::bpp() { return pixelCodec->bpp(); }

uint8_t* RectangleTwiddled::decode(uint8_t* src, uint64_t srcIndex, uint16_t width, uint16_t height) {
    uint8_t* destination = new uint8_t[width * height * 4];
    uint16_t size = std::min(width, height);
    uint64_t* twiddleMap = createTwiddleMap(size);
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

uint8_t* RectangleTwiddled::encode(uint8_t* src, uint64_t srcIndex, uint16_t width, uint16_t height) {
    uint8_t*  destination = new uint8_t[width * height * (pixelCodec->bpp() >> 3)];
    uint64_t destinationIndex = 0;
    uint16_t size = std::min(width, height);
    uint64_t* twiddleMap = createTwiddleMap(size);
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

}
}
