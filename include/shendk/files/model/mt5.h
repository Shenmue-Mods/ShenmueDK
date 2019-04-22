#pragma once

#include <stdint.h>
#include <iostream>
#include <filesystem>

#include "shendk/files/file.h"

namespace shendk {

struct MT5 : File {
	unsigned int signature = 1296257608;

    struct Header {
		char signature[4];
		uint32_t texdOffset;
		uint32_t firstNodeOffset;
    };

    struct Node {
        uint32_t id;
        uint32_t meshOffset;
        uint32_t rotX; // degX = rotX / 0xFFFF * 360.0f
        uint32_t rotY; // degY = rotY / 0xFFFF * 360.0f
        uint32_t rotZ; // degZ = rotZ / 0xFFFF * 360.0f
        float sclX;
        float sclY;
        float sclZ;
        float posX;
        float posY;
        float posZ;
        uint32_t childNodeOffset;
        uint32_t nextNodeOffset;
        uint32_t parentNodeOffset;
        char name[4];
        uint32_t unknown;
    };

	MT5() = default;

	MT5(const std::string& filepath) {
		read(filepath);
	}

	MT5::Header header;

protected:
	virtual void _read(std::istream& stream) {
		int64_t baseOffset = stream.tellg();
		
		std::istream* _stream = &stream;

		_stream->seekg(baseOffset, std::ios::beg);

		// Read header..
		_stream->read(reinterpret_cast<char*>(&header), sizeof(MT5::Header));
		if (!isValid(header.signature))
			throw new std::runtime_error("Invalid signature for MT5 file!\n");

	}

	virtual void _write(std::ostream& stream) {
		stream.write(reinterpret_cast<char*>(&header), sizeof(MT5::Header));
	}

	virtual bool _isValid(unsigned char signature)
	{
		if (this->signature != signature)
			return false;
		return true;
	}

};
}
