#include "shendk/files/container/gz.h"

#include "zlib.h"

namespace shendk {

GZ::GZ() = default;
GZ::GZ(const std::string& filepath) { read(filepath); }
GZ::GZ(std::istream& stream) { read(stream); }

GZ::~GZ() {}

bool GZ::testGzip(std::istream& stream) {
    uint8_t* buffer = new uint8_t[2];
    stream.read(reinterpret_cast<char*>(buffer), 2);
    stream.seekg(0, std::ios::beg);

    z_stream d_stream;
    d_stream.zalloc = static_cast<alloc_func>(nullptr);
    d_stream.zfree = static_cast<free_func>(nullptr);
    d_stream.opaque = static_cast<voidpf>(nullptr);
    d_stream.next_in  = buffer;
    d_stream.avail_in = 0;

    int err;
    err = inflateInit2(&d_stream, 16 + MAX_WBITS);
    delete[] buffer;
    if (err != Z_OK) {
        return false;
    }
    return true;
}

char* GZ::inflateStream(std::istream& inStream, uint64_t& bufferSize) {

    // retrieve input stream size
    inStream.seekg(0, std::ios::end);
    uint32_t inBufferSize = static_cast<uint32_t>(inStream.tellg());
    uint32_t outBufferSize = inBufferSize;
    inStream.seekg(0, std::ios::beg);

    // initialzie buffers
    uint8_t* inBuffer = new uint8_t[inBufferSize];
    uint8_t* outBuffer = new uint8_t[inBufferSize];
    inStream.read(reinterpret_cast<char*>(inBuffer), inBufferSize);

    // initialize decompression stream
    z_stream d_stream;
    d_stream.zalloc = static_cast<alloc_func>(nullptr);
    d_stream.zfree = static_cast<free_func>(nullptr);
    d_stream.opaque = static_cast<voidpf>(nullptr);
    d_stream.next_in  = inBuffer;
    d_stream.next_out  = outBuffer;
    d_stream.avail_in  = inBufferSize;
    d_stream.avail_out = outBufferSize;

    uint32_t ibuflen = inBufferSize;
    uint32_t occupied, newSize;
    int32_t err, flush;

    // initialize and check if input stream is a zlib compatible file
    err = inflateInit2(&d_stream, 16 + MAX_WBITS);
    if (err != Z_OK) {
        return nullptr;
    }

    // decompress stream
    do {
        d_stream.avail_in = ibuflen;
        ibuflen -= d_stream.avail_in;
        flush = ibuflen == 0 ? Z_FINISH : Z_NO_FLUSH;
        do {

            // calculate decompressed bytes
            occupied = static_cast<uint32_t>(d_stream.next_out - outBuffer);

            // increase output buffer size if the end was reached by 2
            if (outBufferSize == occupied) {
                if (outBufferSize == UINT_MAX) return nullptr;
                if (outBufferSize <= (UINT_MAX >> 1)) {
                    newSize = outBufferSize << 1;
                } else {
                    newSize = UINT_MAX;
                }
                outBuffer = reinterpret_cast<uint8_t*>(realloc(outBuffer, newSize));
                if (outBuffer == nullptr) return nullptr;
                outBufferSize = newSize;
            }
            d_stream.avail_out = static_cast<uint32_t>(outBufferSize - occupied);
            d_stream.next_out = outBuffer + occupied;

            // inflate next bytes
            err = inflate(&d_stream, flush);
            switch (err) {
            case Z_OK:
            case Z_BUF_ERROR:
            case Z_STREAM_END:
                break;
            case Z_MEM_ERROR:
                inflateEnd(&d_stream);
                return nullptr;
            default:
                inflateEnd(&d_stream);
                return nullptr;
            }
        } while (d_stream.avail_out == 0);

    } while (err != Z_STREAM_END && ibuflen != 0);

    // cleanup decompression
    bufferSize = d_stream.total_out;
    inflateEnd(&d_stream);
    delete[] inBuffer;

    // allocate new memory that fits the output data
    char* decompressedBuffer = new char[bufferSize];
    memcpy(decompressedBuffer, outBuffer, bufferSize);
    delete[] outBuffer;

    return reinterpret_cast<char*>(decompressedBuffer);
}

void GZ::_read(std::istream& stream) {

}

void GZ::_write(std::ostream& stream) {

}

bool GZ::_isValid(uint32_t signature) {
	return false;
}

}



