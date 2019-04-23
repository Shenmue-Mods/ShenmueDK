#pragma once

#include "shendk/files/texture/texture.h"
#include "shendk/files/texture/pvr/formats.h"
#include "shendk/files/texture/pvr/data_codec.h"
#include "shendk/files/texture/pvr/pixel_codec.h"
#include "shendk/files/texture/pvr/compression_codec.h"
#include "shendk/files/texture/pvr/vector_quantizer.h"

namespace shendk {

struct PVR : Texture {

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

    PVR() = default;

    PVR(const std::string& filepath) {
        read(filepath);
    }

    ~PVR() {}

    PVR::Header header;
    PVR::GBIX globalIndex;

protected:
    virtual void _read(std::istream& stream) {
        int64_t baseOffset = stream.tellg();
        int64_t gbixOffset = 0;
        int64_t pvrtOffset = 0;

        // peek signatures to get pvr structure
        uint32_t signature;
        stream.read(reinterpret_cast<char*>(&signature), sizeof(uint32_t));
        if (signature == gbix) {
            gbixOffset = 0;
            pvrtOffset = sizeof(PVR::GBIX);
        } else {
            stream.read(reinterpret_cast<char*>(&signature), sizeof(uint32_t));
            if (signature == gbix) {
                gbixOffset = 4;
                pvrtOffset = 4 + sizeof(PVR::GBIX);
            } else if (signature == pvrt) {
                gbixOffset = -1;
                pvrtOffset = 4;
            } else {
                gbixOffset = -1;
                pvrtOffset = 0;
            }
        }

        // read gbix node if available
        if (gbixOffset >= 0) {
            stream.seekg(baseOffset + gbixOffset, std::ios::beg);
            stream.read(reinterpret_cast<char*>(&globalIndex), sizeof(GBIX));
        }

        // read pvrt header
        stream.seekg(baseOffset + pvrtOffset, std::ios::beg);
        stream.read(reinterpret_cast<char*>(&header), sizeof(PVR::Header));

        // read image
        if (header.dataFormat == pvr::DataFormat::DDS || header.dataFormat == pvr::DataFormat::DDS_2) {
            throw new std::runtime_error("DDS not implemented!"); // TODO: implement
        } else {
            pvr::PixelCodec* pixelCodec = pvr::getPixelCodec(header.pixelFormat);
            pvr::DataCodec* dataCodec = pvr::getDataCodec(header.dataFormat);
            if (dataCodec != nullptr && pixelCodec != nullptr) {
                dataCodec->pixelCodec = pixelCodec; // TODO: this is bad lazy initialization
            }

            // check for palette
            uint16_t paletteEntries = dataCodec->paletteEntries(header.width);
            int64_t paletteOffset = 0;
            int64_t dataOffset = 0;
            if (paletteEntries == 0 || dataCodec->needsExternalPalette()) {
                paletteOffset = -1;
                dataOffset = pvrtOffset + static_cast<int64_t>(sizeof(PVR::Header));
            } else {
                paletteOffset = pvrtOffset + static_cast<int64_t>(sizeof(PVR::Header));
                dataOffset = paletteOffset + (paletteEntries * (pixelCodec->bpp() >> 3));
            }

            // check for compression
            pvr::CompressionFormat compressionFormat = pvr::CompressionFormat::NONE;
            uint32_t first, second;
            stream.seekg(baseOffset, std::ios::beg);
            stream.read(reinterpret_cast<char*>(&first), sizeof(uint32_t));
            stream.seekg(baseOffset + pvrtOffset + 4, std::ios::beg);
            stream.read(reinterpret_cast<char*>(&second), sizeof(uint32_t));
            if (first == second - pvrtOffset + dataOffset + 8) {
                compressionFormat = pvr::CompressionFormat::RLE;
            }
            pvr::CompressionCodec* compressionCodec = pvr::getCompressionCodec(compressionFormat);
            if (compressionFormat != pvr::CompressionFormat::NONE && compressionCodec != nullptr) {
                throw new std::runtime_error("Compression codecs not implemented");
            }
            delete compressionCodec;

            // get mipmap offsets
            std::vector<int64_t> mipmapOffsets;
            if (dataCodec->hasMipmaps()) {
                int8_t mipmapCount = static_cast<int8_t>(std::log2(header.width) + 1);
                int64_t mipmapOffset = 0;
                if (header.dataFormat == pvr::DataFormat::SQUARE_TWIDDLED_MIPMAP) {
                    mipmapOffset = dataCodec->bpp() >> 3; // A 1x1 mipmap takes up as much space as a 2x1 mipmap
                } else if (header.dataFormat == pvr::DataFormat::SQUARE_TWIDDLED_MIPMAP_ALT) {
                    mipmapOffset = (3 * dataCodec->bpp()) >> 3; // A 1x1 mipmap takes up as much space as a 2x2 mipmap
                }
                for (int i = mipmapCount - 1, size = 1; i >= 0; i--, size <<= 1) {
                    mipmapOffsets.push_back(mipmapOffset);
                    mipmapOffset += std::max((size * size * dataCodec->bpp()) >> 3, 1);
                }
            } else {
                mipmapOffsets.push_back(0);
            }

            // decode palette if available
            if (paletteOffset != -1) {
                stream.seekg(baseOffset + paletteOffset, std::ios::beg);
                dataCodec->setPalette(stream, paletteEntries);
            }

            // decode mipmaps
            mipmaps.clear();
            if (dataCodec->hasMipmaps()) {
                for (int i = 0, size = header.width; i < mipmapOffsets.size(); i++, size >>= 1)
                {
                    stream.seekg(baseOffset + dataOffset + mipmapOffsets[i], std::ios::beg);
                    uint8_t* pixels = dataCodec->decode(stream, size, size, pixelCodec);
                    std::shared_ptr<ArgbImage> mipmap(new ArgbImage(size, size));
                    memcpy(mipmap->getDataPtr(), pixels, mipmap->size());
                    mipmaps.push_back(mipmap);
                    delete[] pixels;
                }
            } else {
                stream.seekg(baseOffset + dataOffset + mipmapOffsets[0], std::ios::beg);
                uint8_t* pixels = dataCodec->decode(stream, header.width, header.height, pixelCodec);
                std::shared_ptr<ArgbImage> mipmap(new ArgbImage(header.width, header.height));
                memcpy(mipmap->getDataPtr(), pixels, mipmap->size());
                mipmaps.push_back(mipmap);
                delete[] pixels;
            }

            // move stream to end of pvr
            stream.seekg(baseOffset + header.size, std::ios::beg);
            if (globalIndex.size != 0) {
                stream.seekg(sizeof(PVR::GBIX), std::ios::cur);
            }

            delete pixelCodec;
            delete dataCodec;
        }
    }

    virtual void _write(std::ostream& stream) {
        int64_t baseOffset = stream.tellp();
    }

    virtual bool _isValid(unsigned int signature)
    {
        // TODO: implement
        return false;
    }
};

}
