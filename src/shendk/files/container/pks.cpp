#include "shendk/files/container/pks.h"

#include "shendk/files/container/gz.h"
#include "shendk/utils/memstream.h"

namespace shendk {

PKS::PKS() = default;
PKS::PKS(std::istream& stream) { read(stream); }
PKS::PKS(const std::string& filepath) { read(filepath); }
PKS::~PKS() {}

void PKS::_read(std::istream& stream) {
    std::istream* _stream = &stream;

    if (GZ::testGzip(stream)) {
        uint64_t bufferSize;
        char* decompressed = GZ::inflateStream(stream, bufferSize);
        if (decompressed == nullptr) {
            return;
        }
        _stream = new imstream(decompressed, bufferSize);
    } else {
        _stream->seekg(baseOffset, std::ios::beg);
    }

    _stream->read(reinterpret_cast<char*>(&header), sizeof(PKS::Header));
    if (!isValid(header.signature))
        throw new std::runtime_error("Invalid signature for PKS file!\n");

    ipac.read(*_stream);
}

void PKS::_write(std::ostream& stream) {
    stream.write(reinterpret_cast<char*>(&header), sizeof(PKS::Header));
    ipac.write(stream);
}

bool PKS::_isValid(uint32_t signature)
{
    if (signature != PKS::signature)
        return false;
    else return true;
}

}
