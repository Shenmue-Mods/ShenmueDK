#pragma once

#include "shendk/files/model/mt5/mt5_node.h"
#include "shendk/files/model/mt5/mt5_instruction.h"

namespace shendk {
namespace mt5 {

struct MT5Mesh : public NodeMesh {

    struct Header {
        uint32_t polyType;
        uint32_t verticesOffset;
        int32_t vertexCount;
        uint32_t facesOffset;
        float centerX;
        float centerY;
        float centerZ;
        float radius;
    };

    MT5Mesh(MT5Node* node);
    MT5Mesh(MT5Node* node, std::istream& stream);
    ~MT5Mesh();

    void read(std::istream& stream);
    void write(std::ostream& stream);

    MT5Mesh::Header header;
    MT5Node* parentNode = nullptr;

    State state;
    std::vector<std::shared_ptr<Instruction>> instructions;
};

}
}
