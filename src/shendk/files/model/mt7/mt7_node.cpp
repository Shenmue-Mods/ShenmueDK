#include "shendk/files/model/mt7/mt7_node.h"
#include "shendk/files/model/mt7/mt7_mesh.h"
#include "shendk/utils/math.h"

#include <stdexcept>
#include <unordered_set>

namespace shendk::mt7 {
namespace {
thread_local size_t nodeReadDepth = 0;
thread_local std::unordered_set<int64_t> visitedNodes;
struct NodeReadScope {
    NodeReadScope(int64_t offset) {
        if (!nodeReadDepth)
            visitedNodes.clear();
        if (nodeReadDepth >= 512 || !visitedNodes.insert(offset).second)
            throw std::runtime_error("Invalid MT7 node hierarchy");
        ++nodeReadDepth;
    }
    ~NodeReadScope() { --nodeReadDepth; }
};
}

MT7Node::MT7Node(Model *model, MT7Node *owner) : ModelNode(model) { parent = owner; }
MT7Node::MT7Node(Model *model, std::istream &stream, int64_t base, MT7Node *owner) : ModelNode(model) {
    parent = owner;
    read(stream, base);
}
MT7Node::~MT7Node() = default;

void MT7Node::read(std::istream &stream, int64_t base) {
    const int64_t offset = stream.tellg();
    NodeReadScope scope(offset);
    sourceBaseOffset = base;
    stream.read(reinterpret_cast<char *>(&data), sizeof(data));
    if (!stream)
        throw std::runtime_error("Truncated MT7 node");
    id = data.id;
    position = Vector3f(data.position[0], data.position[1], data.position[2]);
    rotation = Vector3f(shortToDegrees(static_cast<int16_t>(data.rotation[0])), shortToDegrees(static_cast<int16_t>(data.rotation[1])),
        shortToDegrees(static_cast<int16_t>(data.rotation[2])));
    scale = Vector3f(data.scale[0], data.scale[1], data.scale[2]);
    XB01Offset = data.meshOffset;
    ChildOffset = data.childOffset;
    SiblingOffset = data.siblingOffset;
    ParentOffset = data.parentOffset;
    if (XB01Offset) {
        stream.seekg(base + XB01Offset, std::ios::beg);
        mesh = new MT7Mesh(this, stream);
        HasMesh = true;
    }
    if (ChildOffset) {
        stream.seekg(base + ChildOffset, std::ios::beg);
        child = new MT7Node(model, stream, base, this);
    }
    if (SiblingOffset) {
        stream.seekg(base + SiblingOffset, std::ios::beg);
        nextSibling = new MT7Node(model, stream, base, static_cast<MT7Node *>(parent));
    }
}

} // namespace shendk::mt7
