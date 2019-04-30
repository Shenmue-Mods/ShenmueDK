#include "shendk/files/container/pkf.h"

#include "shendk/files/container/gz.h"
#include "shendk/node/dumy.h"
#include "shendk/utils/memstream.h"

namespace shendk {

PKF::PKF() = default;
PKF::PKF(std::istream& stream) { read(stream); }
PKF::PKF(const std::string& filepath) { read(filepath); }

PKF::~PKF() {}

void PKF::_read(std::istream& stream) {
    std::istream* _stream = &stream;

    // decompress if necessary
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

    // read header
    _stream->read(reinterpret_cast<char*>(&header), sizeof(PKF::Header));
    if (!isValid(header.signature))
        throw new std::runtime_error("Invalid signature for PKF file!\n");

    // check for DUMY entry
    Node::Header dummyEntry;
    _stream->read(reinterpret_cast<char*>(&dummyEntry), sizeof(Node::Header));
    _stream->seekg(baseOffset + sizeof(PKF::Header), std::ios::beg);
    if (dummyEntry.signature == 0x594D5544) {
        DUMY dumy(*_stream);
    }

    // read texture entries
    for (uint32_t i = 0; i < header.fileCount; i++) {
        if (_stream->eof()) break;
        TEXN entry(*_stream);
        textures.push_back(entry);
    }

    // read ipac if necessary
    _stream->seekg(baseOffset + header.contentSize, std::ios::beg);
    if (!stream.eof()) {
        ipac = new IPAC();
        ipac->read(*_stream);
    }
}

void PKF::_write(std::ostream& stream) {
    // skip header
    stream.seekp(sizeof(PKF::Header), std::ios::cur);

    // write dumy
    DUMY dummy;
    dummy.write(stream);

    // write texture entries
    for (auto& entry : textures) {
        entry.write(stream);
    }

    // update headers
    header.fileCount = static_cast<uint32_t>(textures.size());
    header.contentSize = static_cast<uint32_t>(stream.tellp() - baseOffset);

    // write header
    stream.seekp(baseOffset, std::ios::beg);
    stream.write(reinterpret_cast<char*>(&header), sizeof(PKF::Header));
    stream.seekp(baseOffset + header.contentSize, std::ios::beg);

    // write ipac if necessary
    if (ipac != nullptr) {
        ipac->write(stream);
    }
}

bool PKF::_isValid(uint32_t signature) {
    if (signature != PKF::signature)
        return false;
    else return true;
}

}
