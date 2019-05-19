#include "shendk/files/model/mt5/mt5_node.h"

#include "shendk/files/model/mt5/mt5_mesh.h"
#include "shendk/utils/math.h"

#include <iostream>

namespace shendk {
namespace mt5 {

MT5Node::MT5Node(Model* model, MT5Node* _parent)
    : ModelNode(model)
{
    parent = _parent;
}

MT5Node::MT5Node(Model* model, std::istream& stream, int64_t baseOffset, MT5Node* _parent)
    : ModelNode(model)
{
    parent = _parent;
    read(stream, baseOffset);
}

MT5Node::~MT5Node() {}

void MT5Node::read(std::istream& stream , int64_t baseOffset) {
    stream.read(reinterpret_cast<char*>(&data), sizeof(MT5Node::Data));

    id = data.id;
    position = Vector3f(data.posX, data.posY, data.posZ);
    scale = Vector3f(data.sclX, data.sclY, data.sclZ);
    rotation = Vector3f(ushortToDegrees(data.rotX), ushortToDegrees(data.rotY), ushortToDegrees(data.rotZ));

    // read mesh data
    if (data.meshOffset != 0) {
        stream.seekg(baseOffset + data.meshOffset, std::ios::beg);
        mesh = new MT5Mesh(this, stream);
    }

    // construct nodes
    if (data.childNodeOffset != 0) {
        stream.seekg(baseOffset + data.childNodeOffset, std::ios::beg);
        child = new MT5Node(model, stream, baseOffset, this);
    }

    if (data.nextNodeOffset != 0) {
        stream.seekg(baseOffset + data.nextNodeOffset, std::ios::beg);
        nextSibling = new MT5Node(model,stream, baseOffset, dynamic_cast<MT5Node*>(parent));
    }
}

void MT5Node::write(std::ostream& stream) {
    data.id = id;
    data.posX = position.x;
    data.posY = position.y;
    data.posZ = position.z;
    data.rotX = degreesToUshort(rotation.x);
    data.rotY = degreesToUshort(rotation.y);
    data.rotZ = degreesToUshort(rotation.z);
    data.sclX = scale.x;
    data.sclY = scale.y;
    data.sclZ = scale.z;

}


}
}
