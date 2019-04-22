#pragma once

#include <stdint.h>

#include "shendk/files/file.h"

namespace shendk {

struct MT7 : File {

    struct Header {
        uint32_t signature;
        uint32_t size;
        uint32_t firstNodeOffset;
        uint32_t textureCount;
    };


    struct Node {
        uint32_t id;
        float posX;
        float posY;
        float posZ;
        uint32_t rotX; // degX = rotX / 0xFFFF * 360.0f
        uint32_t rotY; // degY = rotY / 0xFFFF * 360.0f
        uint32_t rotZ; // degZ = rotZ / 0xFFFF * 360.0f
        float sclX;
        float sclY;
        float sclZ;
        uint32_t xb01Offset;
        uint32_t childNodeOffset;
        uint32_t nextNodeOffset;
        uint32_t parentNodeOffset;
    };

	MT7() = default;

	MT7(const std::string& filepath) {
		read(filepath);
	}

	MT7::Header header;
protected:
	virtual void _read(std::istream& stream) {
		int64_t baseOffset = stream.tellg();

		std::istream* _stream = &stream;

		_stream->seekg(baseOffset, std::ios::beg);

		// Read header..
		_stream->read(reinterpret_cast<char*>(&header), sizeof(MT7::Header));
	}

	virtual void _write(std::ostream& stream) {
		stream.write(reinterpret_cast<char*>(&header), sizeof(MT7::Header));
	}

	virtual bool _isValid(unsigned char signature)
	{
		
	}
};

}
