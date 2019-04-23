#pragma once

#include <stdint.h>
#include <iostream>
#include <filesystem>

#include "shendk/files/model/model.h"
#include "shendk/files/model/mt5/meshdata.h"

namespace shendk {

struct MT5Node : ModelNode {

    struct Data {
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

    MT5Node() {}

    MT5Node(std::istream& stream) {
        read(stream);
    }

    virtual ~MT5Node() {}

    void read(std::istream& stream) {
        stream.read(reinterpret_cast<char*>(&data), sizeof(MT5Node::Data));
        int64_t offset = stream.tellg();

        id = data.id;
        position = Eigen::Vector3f(data.posX, data.posY, data.posZ);
        scale = Eigen::Vector3f(data.sclX, data.sclY, data.sclZ);
        rotation = Eigen::Vector3f(ushortToDegrees(data.rotX), ushortToDegrees(data.rotY), ushortToDegrees(data.rotZ));

        // read mesh data
        if (data.meshOffset != 0) {
            stream.seekg(data.meshOffset, std::ios::beg);
            meshdata = new mt5::MeshData(stream, this);
        }

        // construct nodes
        if (data.childNodeOffset != 0) {
            stream.seekg(data.childNodeOffset, std::ios::beg);
            child = new MT5Node(stream);
        }

        if (data.nextNodeOffset != 0) {
            stream.seekg(data.nextNodeOffset, std::ios::beg);
            nextSibling = new MT5Node(stream);
        }

        stream.seekg(offset, std::ios::beg);
    }

    void write(std::ostream& stream) {

    }

    MT5Node::Data data;
    mt5::MeshData* meshdata;
};

struct MT5 : Model {
    const unsigned int signature = 1296257608;

    struct Header {
        uint32_t signature;
		uint32_t texdOffset;
		uint32_t firstNodeOffset;
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
        if (!isValid(header.signature)) throw new std::runtime_error("Invalid signature for MT5 file!\n");



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
