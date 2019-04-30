#pragma once

#include <stdint.h>
#include <cstring>
#include <algorithm>
#include <iostream>

#include "shendk/files/image/pvr/formats.h"
#include "shendk/files/image/pvr/data_codec.h"


namespace shendk {
namespace pvr {

struct CompressionCodec {
    virtual ~CompressionCodec();
    virtual uint8_t* decompress(uint8_t* input, uint32_t inputLength, uint64_t DataOffset, DataCodec* DataCodec) = 0;
    virtual uint8_t* compress(uint8_t* input, uint32_t inputLength, uint64_t DataOffset, DataCodec* DataCodec) = 0;

    static CompressionCodec* getCompressionCodec(CompressionFormat format);
};

struct RLE : CompressionCodec {
    uint8_t* decompress(uint8_t* input, uint32_t inputLength, uint64_t dataOffset, DataCodec* dataCodec);
    uint8_t* compress(uint8_t* input, uint32_t inputLength, uint64_t dataOffset, DataCodec* dataCodec);
};

}
}
