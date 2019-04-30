#pragma once

#include <stdint.h>
#include <vector>
#include <memory>

#include "shendk/types/vector.h"
#include "shendk/types/matrix.h"

#include "shendk/files/file.h"
#include "shendk/types/texture.h"

namespace shendk {

enum class BoneID : uint8_t {
    Spine = 1,
    Hips = 14,
    RightUpperLeg = 16,
    RightLowerLeg = 17,
    RightFoot = 18,
    LeftUpperLeg = 21,
    LeftLowerLeg = 22,
    LeftFoot = 23,
    RightShoulder = 4,
    RightUpperArm = 5,
    RightLowerArm = 6,
    RightWrist = 7,
    RightHand = 191,
    LeftShoulder = 9,
    LeftUpperArm = 10,
    LeftLowerArm = 11,
    LeftWrist = 12,
    LeftHand = 190,
    Head = 189,
    Jaw = 188,
    None = 0xFF
};

enum class TypeID : uint8_t {
    Unknown = 0x00, // Sometimes used by character bones (still rendered tho)
    Static = 0xFF
};

enum class LayerID : uint8_t {
    Static = 0x00,   // Always rendered
    Trigger = 0x02,  // Not rendered
    Unknown = 0x06,  // Mesh fence, unusable door, background pipes
    Unknown2 = 0x09, // Out of bounds terrain,
    Unknown3 = 0x04, // Lamp mesh, Roof flag post, some wall?, some windows?
    Unknown4 = 0x11, // Lamps
    Unknown5 = 0x14, // windows
    Unknown6 = 0x16, // background meshes
};

enum class TextureWrapMode {
    Clamp,
    Repeat,
    MirroredRepeat
};

enum class PrimitiveType {
    Triangles,
    TriangleStrip
};

struct Material {
    uint32_t textureIndex;
    Texture* texture;
    TextureWrapMode textureWrapMode;
    bool unlit;
    bool transparent;
};

struct MeshSurface {
    PrimitiveType type;
    Material material;
    std::vector<uint16_t> positionIndices;
    std::vector<uint16_t> normalIndices;
    std::vector<uint16_t> uvIndices;
    std::vector<uint16_t> colorIndices;
};

struct ModelNode {

    virtual ~ModelNode();

    std::vector<ModelNode*> getAllNodes(bool includeSibling = true, bool includeChildren = true);
    Matrix4x4f getTransformMatrixSelf();
    Matrix4x4f getTransformMatrix();

    uint32_t id;

    Vector3f position;
    Vector3f rotation;
    Vector3f scale;
    Vector3f center;
    float radius;

    ModelNode* child = nullptr;
    ModelNode* nextSibling = nullptr;
    ModelNode* parent = nullptr;

    std::vector<Vector3f> vertexPositions;
    std::vector<Vector3f> vertexNormals;
    std::vector<Vector2f> vertexUVs;
    std::vector<Vector4f> vertexColors;
    std::vector<MeshSurface> surfaces;

};

struct Model {

    // Model ID = {0x00;LayerID;TypeID;BoneID}
    ModelNode* rootNode;
    std::vector<std::shared_ptr<Texture>> textures;
};



}
