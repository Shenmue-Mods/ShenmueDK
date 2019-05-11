#pragma once

#include "shendk/types/model.h"

namespace shendk {
namespace mt5 {

struct MT5Mesh;

struct MT5Node : public ModelNode {

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

    MT5Node();
    MT5Node(std::istream& stream, int64_t baseOffset, MT5Node* par = nullptr);
    virtual ~MT5Node();

    void read(std::istream& stream , int64_t baseOffset);
    void write(std::ostream& stream);

    MT5Node::Data data;
};

}
}
