#pragma once

#include "shendk/types/model.h"

namespace shendk {
namespace mt5 {

struct MT5Mesh;

struct MT5Node : public ModelNode {

    struct Data {
        uint32_t id;
        uint32_t meshOffset;
        int32_t rotX;
        int32_t rotY;
        int32_t rotZ;
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

    MT5Node(Model* model, MT5Node* parent = nullptr);
    MT5Node(Model* model, std::istream& stream, int64_t baseOffset, MT5Node* parent = nullptr);
    virtual ~MT5Node();

    void read(std::istream& stream , int64_t baseOffset);
    void write(std::ostream& stream);

    MT5Node::Data data;
    int64_t sourceBaseOffset = 0;
};

}
}
