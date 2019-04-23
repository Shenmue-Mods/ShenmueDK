#pragma once

#include <stdint.h>
#include <cstring>
#include <algorithm>
#include <iostream>

#include "shendk/utils/math.h"
#include "shendk/utils/memstream.h"
#include "shendk/files/texture/pvr/formats.h"
#include "shendk/files/texture/pvr/data_codec.h"


namespace shendk {
namespace pvr {

// TODO: implement (not needed for shenmue at the moment)

struct CompressionCodec {

    virtual ~CompressionCodec() {}

    virtual uint8_t* decompress(uint8_t* input, uint32_t inputLength, uint64_t DataOffset, DataCodec* DataCodec) = 0;
    virtual uint8_t* compress(uint8_t* input, uint32_t inputLength, uint64_t DataOffset, DataCodec* DataCodec) = 0;

};

struct RLE : CompressionCodec {

    uint8_t* decompress(uint8_t* input, uint32_t inputLength, uint64_t dataOffset, DataCodec* dataCodec) {
        /*
        uint32_t outputLength = *reinterpret_cast<uint32_t*>(input);
        uint8_t* output     = new uint8_t[outputLength];
        uint64_t sourcePointer = dataOffset;
        uint64_t destPointer   = 0x00;
        uint16_t pixelSize     = (dataCodec->bpp() >> 3);

        if (dataOffset - 4 > 0) {
            Array.Copy(input, 0x04, output, 0x00, dataOffset - 4);
            destPointer += (dataOffset - 4);
        }

        while (sourcePointer < inputLength && destPointer < outputLength) {
            int amount = input[sourcePointer + pixelSize] + 1;
            for (int i = 0; i < amount; i++) {
                Array.Copy(input, sourcePointer, output, destPointer, pixelSize);
                destPointer += pixelSize;
            }
            sourcePointer += pixelSize + 1;
        }
        return output;*/
        return nullptr;
    }

    uint8_t* compress(uint8_t* input, uint32_t inputLength, uint64_t dataOffset, DataCodec* dataCodec) {
        /*if (dataCodec->bpp() < 8) return input;
        int sourcePointer   = dataOffset;
        int destPointer     = dataOffset + 4;
        int pixelSize = (dataCodec->bpp() >> 3);

        omstream output;
        Writer.Write(inputLength);
        Writer.Write(input, 0x00, dataOffset);
        while (sourcePointer < inputLength)
        {
            uint8_t* pixel = new uint8_t[pixelSize];
            Array.Copy(input, sourcePointer, pixel, 0x00, pixelSize);
            Writer.Write(pixel);
            sourcePointer += pixelSize;
            destPointer   += pixelSize;

            int repeat = 0;
            while (SourcePointer + PixelSize < inputLength && repeat < 255)
            {
                bool match = true;

                for (int i = 0; i < PixelSize && match; i++)
                {
                    if (input[SourcePointer + i] != pixel[i])
                    {
                        match = false;
                        break;
                    }
                }

                if (match)
                {
                    repeat++;
                    SourcePointer += PixelSize;
                }
                else
                    break;
            }

            Writer.Write((byte)repeat);
            DestPointer++;
        }

        Writer.Flush();


        return output.ToArray();*/
        return nullptr;
    }

};

static CompressionCodec* getCompressionCodec(CompressionFormat format)
{
    switch (format)
    {
        case CompressionFormat::RLE:
            return new RLE();
        default:
            return nullptr;
    }
}

}
}
