#pragma once

#include <stdint.h>
#include <vector>
#include <iostream>
#include <filesystem>

#include "shendk/files/file.h"
#include "shendk/files/container/ipac.h"
#include "shendk/files/container/gz.h"
#include "shendk/utils/memstream.h"

namespace fs = std::filesystem;

namespace shendk {

struct PKS : File {
	unsigned int signature = 1397440848;

    struct Header {
        uint32_t signature;
        uint32_t ipacOffset;
        uint32_t unknown1;
        uint32_t unknown2;
    };

    PKS() = default;
    PKS(std::istream& stream) { read(stream); }
    PKS(const std::string& filepath) { read(filepath); }

    ~PKS() {}

    PKS::Header header;
    IPAC ipac;

protected:
    virtual void _read(std::istream& stream) {
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

    virtual void _write(std::ostream& stream) {
        stream.write(reinterpret_cast<char*>(&header), sizeof(PKS::Header));
        ipac.write(stream);
    }

	virtual bool _isValid(unsigned int signature)
	{
		if (signature != PKS::signature)
			return false;
		else return true;
	}
};
}
