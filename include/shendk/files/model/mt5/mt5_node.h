#pragma once

#include "shendk/types/model.h"
#include "shendk/files/model/mt5/mt5_mesh.h"

namespace shendk {
namespace mt5 {

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

    MT5Node(std::istream& stream, int64_t baseOffset) {
        read(stream, baseOffset);
    }

    virtual ~MT5Node() {}

    void read(std::istream& stream , int64_t baseOffset) {
        stream.read(reinterpret_cast<char*>(&data), sizeof(MT5Node::Data));

        id = data.id;
        position = Eigen::Vector3f(data.posX, data.posY, data.posZ);
        scale = Eigen::Vector3f(data.sclX, data.sclY, data.sclZ);
        rotation = Eigen::Vector3f(ushortToDegrees(data.rotX), ushortToDegrees(data.rotY), ushortToDegrees(data.rotZ));

        // read mesh data
        if (data.meshOffset != 0) {
            stream.seekg(baseOffset + data.meshOffset, std::ios::beg);
            meshdata = new MT5Mesh(stream, this);
        }

        // construct nodes
        if (data.childNodeOffset != 0) {
            stream.seekg(baseOffset + data.childNodeOffset, std::ios::beg);
            child = new MT5Node(stream, baseOffset);
        }

        if (data.nextNodeOffset != 0) {
            stream.seekg(baseOffset + data.nextNodeOffset, std::ios::beg);
            nextSibling = new MT5Node(stream, baseOffset);
        }
    }

    void write(std::ostream& stream) {

    }

    MT5Node::Data data;
    mt5::MT5Mesh* meshdata;

};

}
}
