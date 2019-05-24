#pragma once

#include <stdint.h>
#include <vector>
#include <memory>
#include <map>

#include "shendk/types/vector.h"
#include "shendk/types/matrix.h"

#include "shendk/files/file.h"
#include "shendk/types/texture.h"

namespace shendk {

enum class BoneID : uint8_t {
    Spine = 1,
    Hip = 14,
    RightUpperLeg = 16,
    RightLowerLeg = 17,
    RightFoot = 18,
    RightFootToes = 19,
    LeftUpperLeg = 21,
    LeftLowerLeg = 22,
    LeftFoot = 23,
    LeftFootToes = 24,
    RightShoulder = 4,
    RightUpperArm = 5,
    RightLowerArm = 6,
    RightWrist = 7,
    RightHand = 191,
    RightHandIndexUpper = 28,
    RightHandIndexLower = 29,
    RightHandFingerUpper = 31,
    RightHandFingerLower = 32,
    RightHandThumb = 25,
    LeftShoulder = 9,
    LeftUpperArm = 10,
    LeftLowerArm = 11,
    LeftWrist = 12,
    LeftHand = 190,
    LeftHandIndexUpper = 43,
    LeftHandIndexLower = 44,
    LeftHandFingerUpper = 46,
    LeftHandFingerLower = 47,
    LeftHandThumb = 40,
    Head = 189,
    Jaw = 188,
    None = 0xFF
};

static std::map<BoneID, std::string> BoneName = {
    {BoneID::Spine, "Spine"},
    {BoneID::Hip, "Hip"},
    {BoneID::RightUpperLeg, "RightUpperLeg"},
    {BoneID::RightLowerLeg, "RightLowerLeg"},
    {BoneID::RightFoot, "RightFoot"},
    {BoneID::RightFootToes, "RightFootToes"},
    {BoneID::LeftUpperLeg, "LeftUpperLeg"},
    {BoneID::LeftLowerLeg, "LeftLowerLeg"},
    {BoneID::LeftFoot, "LeftFoot"},
    {BoneID::LeftFootToes, "LeftFootToes"},
    {BoneID::RightShoulder, "RightShoulder"},
    {BoneID::RightUpperArm, "RightUpperArm"},
    {BoneID::RightLowerArm, "RightLowerArm"},
    {BoneID::RightWrist, "RightWrist"},
    {BoneID::RightHand, "RightHand"},
    {BoneID::RightHandIndexUpper, "RightHandIndexUpper"},
    {BoneID::RightHandIndexLower, "RightHandIndexLower"},
    {BoneID::RightHandFingerUpper, "RightHandFingerUpper"},
    {BoneID::RightHandFingerLower, "RightHandFingerLower"},
    {BoneID::RightHandThumb, "RightHandThumb"},
    {BoneID::LeftShoulder, "LeftShoulder"},
    {BoneID::LeftUpperArm, "LeftUpperArm"},
    {BoneID::LeftLowerArm, "LeftLowerArm"},
    {BoneID::LeftWrist, "LeftWrist"},
    {BoneID::LeftHand, "LeftHand"},
    {BoneID::LeftHandIndexUpper, "LeftHandIndexUpper"},
    {BoneID::LeftHandIndexLower, "LeftHandIndexLower"},
    {BoneID::LeftHandFingerUpper, "LeftHandFingerUpper"},
    {BoneID::LeftHandFingerLower, "LeftHandFingerLower"},
    {BoneID::LeftHandThumb, "LeftHandThumb"},
    {BoneID::Head, "Head"},
    {BoneID::Jaw, "Jaw"},
    {BoneID::None, "None"}
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
    std::shared_ptr<Texture> texture;
    TextureWrapMode textureWrapMode;
    bool unlit;
    bool transparent;
};


/** @brief Vertex buffer containing each vertex attribute as own array of varying lengths.
 *         This is due to the fact that a single vertex can have the same position and normal,
 *         but not the same texture coordinates. */
struct VertexBuffer {
    std::vector<Vector3f> positions;
    std::vector<Vector3f> normals;
    std::vector<Vector2f> texcoords;
    std::vector<Vector4f> colors;
    std::vector<float> weights;
    std::vector<BoneID> joints;

    /** @brief transformed positions */
    std::vector<Vector3f> t_positions;
    /** @brief transformed normals */
    std::vector<Vector3f> t_normals;

    uint32_t vertexCount() { return static_cast<uint32_t>(positions.size()); }

    // byte arrays getter (for glTF)
    std::vector<uint8_t> getPositionData(Matrix4f matrix = Matrix4f::identity());
    std::vector<uint8_t> getNormalData(Matrix4f matrix = Matrix4f::identity());
    std::vector<uint8_t> getTexcoordData();
    std::vector<uint8_t> getColorData();
    std::vector<uint8_t> getWeightsData();
    std::vector<uint8_t> getJointsData();
};

struct MeshSurface {
    PrimitiveType type;
    Material material;
    std::vector<uint32_t> positionIndices;
    std::vector<uint32_t> normalIndices;
    std::vector<uint32_t> texcoordIndices;
    std::vector<uint32_t> colorIndices;
    std::vector<uint32_t> weightIndices;
    std::vector<uint32_t> jointIndices;

    int indexCount() { return positionIndices.size(); }
    bool hasPosition() { return positionIndices.size() > 0; }
    bool hasNormal() { return normalIndices.size() > 0; }
    bool hasTexcoord() { return texcoordIndices.size() > 0; }
    bool hasColor() { return colorIndices.size() > 0; }
    bool hasWeight() { return weightIndices.size() > 0; }
    bool hasJoint() { return jointIndices.size() > 0; }

    int lengthBytes() { return stride() * indexCount(); }

    int stride() {
        int stride = 0;
        if (hasPosition()) {
            stride += sizeof(Vector3f);
        }
        if (hasNormal()) {
            stride += sizeof(Vector3f);
        }
        if (hasTexcoord()) {
            stride += sizeof(Vector2f);
        }
        /*if (hasColor()) {
            stride += sizeof(Vector4f);
        }*/
        return stride;
    }

    void mergeSurface(MeshSurface& rhs);
    void convertToTriangles();
};


struct ModelNode;

struct NodeMesh {

    NodeMesh(ModelNode* node);

    ModelNode* node;
    uint32_t vertexCount = 0;
    uint32_t vertexBufferOffset = 0;

    std::vector<MeshSurface> surfaces;

    VertexBuffer& vertexBuffer();
    uint32_t getIndex(int32_t index);
};

struct Model;

struct ModelNode : std::enable_shared_from_this<ModelNode> {

    ModelNode(Model* model);
    virtual ~ModelNode();

    std::vector<ModelNode*> getAllNodes(bool includeSibling = true, bool includeChildren = true);
    Matrix4f getTransformMatrixSelf();
    Matrix4f getTransformMatrix();

    BoneID getBoneID();

    Model* model;

    uint32_t id;
    Vector3f position;
    Vector3f rotation;
    Vector3f scale;
    Vector3f center;
    float radius;

    ModelNode* child = nullptr;
    ModelNode* nextSibling = nullptr;
    ModelNode* parent = nullptr;

    NodeMesh* mesh = nullptr;

    std::string name = "";
};

struct Model {
    std::shared_ptr<ModelNode> rootNode;
    std::vector<Texture> textures;
    VertexBuffer vertexBuffer;
};

struct Animation {

};

struct KeyFrame {

};

struct Sequence {

};

}
