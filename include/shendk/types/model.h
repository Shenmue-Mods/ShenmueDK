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

struct MeshSurface {
    PrimitiveType type;
    Material material;
    std::vector<uint16_t> positionIndices;
    std::vector<uint16_t> normalIndices;
    std::vector<uint16_t> uvIndices;
    std::vector<uint16_t> colorIndices;

    int indexCount() { return positionIndices.size(); }
    bool hasPosition() { return positionIndices.size() > 0; }
    bool hasNormal() { return normalIndices.size() > 0; }
    bool hasUV() { return uvIndices.size() > 0; }
    bool hasColor() { return colorIndices.size() > 0; }

    int lengthBytes() { return stride() * indexCount(); }

    int stride() {
        int stride = 0;
        if (hasPosition()) {
            stride += sizeof(Vector3f);
        }
        if (hasNormal()) {
            stride += sizeof(Vector3f);
        }
        if (hasUV()) {
            stride += sizeof(Vector2f);
        }
        /*if (hasColor()) {
            stride += sizeof(Vector4f);
        }*/
        return stride;
    }

    void mergeSurface(MeshSurface& rhs) {
        if (type != PrimitiveType::Triangles) {
            throw new std::runtime_error("Only triangle surfaces can be merged.");
        }
        if (hasPosition()) {
            positionIndices.insert(positionIndices.end(), rhs.positionIndices.begin(), rhs.positionIndices.end());
        }
        if (hasNormal()) {
            normalIndices.insert(normalIndices.end(), rhs.normalIndices.begin(), rhs.normalIndices.end());
        }
        if (hasUV()) {
            uvIndices.insert(uvIndices.end(), rhs.uvIndices.begin(), rhs.uvIndices.end());
        }
        if (hasColor()) {
            colorIndices.insert(colorIndices.end(), rhs.colorIndices.begin(), rhs.colorIndices.end());
        }
    }

    void convertToTriangles() {
        if (hasPosition()) {
            std::vector<uint16_t> newIndices;
            for (int i = 0; i < indexCount() - 2; i++) {
                int index1 = positionIndices[i];
                int index2 = positionIndices[i + 1];
                int index3 = positionIndices[i + 2];
                if ((i & 1) == 1) {
                    newIndices.push_back(index1);
                    newIndices.push_back(index2);
                    newIndices.push_back(index3);
                } else {
                    newIndices.push_back(index1);
                    newIndices.push_back(index3);
                    newIndices.push_back(index2);
                }
            }
            positionIndices = newIndices;
        }
        if (hasNormal()){
            std::vector<uint16_t> newIndices;
            for (int i = 0; i < indexCount() - 2; i++) {
                int index1 = normalIndices[i];
                int index2 = normalIndices[i + 1];
                int index3 = normalIndices[i + 2];
                if ((i & 1) == 1) {
                    newIndices.push_back(index1);
                    newIndices.push_back(index2);
                    newIndices.push_back(index3);
                } else {
                    newIndices.push_back(index1);
                    newIndices.push_back(index3);
                    newIndices.push_back(index2);
                }
            }
            normalIndices = newIndices;
        }
        if (hasUV()){
            std::vector<uint16_t> newIndices;
            for (int i = 0; i < indexCount() - 2; i++) {
                int index1 = uvIndices[i];
                int index2 = uvIndices[i + 1];
                int index3 = uvIndices[i + 2];
                if ((i & 1) == 1) {
                    newIndices.push_back(index1);
                    newIndices.push_back(index2);
                    newIndices.push_back(index3);
                } else {
                    newIndices.push_back(index1);
                    newIndices.push_back(index3);
                    newIndices.push_back(index2);
                }
            }
            uvIndices = newIndices;
        }
        if (hasColor()){
            std::vector<uint16_t> newIndices;
            for (int i = 0; i < indexCount() - 2; i++) {
                int index1 = colorIndices[i];
                int index2 = colorIndices[i + 1];
                int index3 = colorIndices[i + 2];
                if ((i & 1) == 1) {
                    newIndices.push_back(index1);
                    newIndices.push_back(index2);
                    newIndices.push_back(index3);
                } else {
                    newIndices.push_back(index1);
                    newIndices.push_back(index3);
                    newIndices.push_back(index2);
                }
            }
            colorIndices = newIndices;
        }
        type = PrimitiveType::Triangles;
    }
};

struct NodeMesh {
    uint32_t vertexCount;

    std::vector<Vector3f> vertexPositions;
    std::vector<Vector3f> vertexNormals;

    std::vector<Vector2f> vertexUVs;
    std::vector<Vector4f> vertexColors;
    std::vector<MeshSurface> surfaces;

    std::vector<uint8_t> getIndicesData() {
        std::vector<uint8_t> data;
        uint16_t index = 0;
        for (auto& surface : surfaces) {
            int vertCount = surface.positionIndices.size();
            for (int i = 0; i < vertCount; i++) {
                for (int j = 0; j < sizeof(uint16_t); j++) {
                    data.push_back(reinterpret_cast<uint8_t*>(&index)[j]);
                }
                index++;
            }
            index = 0;
        }
        return data;
    }

    std::vector<uint8_t> createJointData(uint16_t jointID) {
        std::vector<uint8_t> data;
        for (auto& surface : surfaces) {
            int vertCount = surface.positionIndices.size();
            for (int i = 0; i < vertCount; i++) {
                for (int c = 0; c < 4; c++) {
                    for (int j = 0; j < sizeof(uint16_t); j++) {
                        data.push_back(reinterpret_cast<uint8_t*>(&jointID)[j]);
                    }
                }
            }
        }
        return data;
    }

    std::vector<uint8_t> createWeightData(Vector4f weight) {
        std::vector<uint8_t> data;
        for (auto& surface : surfaces) {
            int vertCount = surface.positionIndices.size();
            for (int i = 0; i < vertCount; i++) {
                for (int j = 0; j < sizeof(Vector4f); j++) {
                    data.push_back(reinterpret_cast<uint8_t*>(&weight)[j]);
                }
            }
        }
        return data;
    }

    std::vector<uint8_t> getPositionData(Matrix4f matrix = Matrix4f::identity()) {
        std::vector<uint8_t> data;
        for (auto& surface : surfaces) {
            int vertCount = surface.positionIndices.size();
            bool hasPosition = surface.hasPosition();
            for (int i = 0; i < vertCount; i++) {
                Vector3f pos;
                if (hasPosition) {
                    pos = vertexPositions[surface.positionIndices[i]];
                    pos = pos.transformPosition(matrix);
                }
                for (int j = 0; j < sizeof(Vector3f); j++) {
                    data.push_back(reinterpret_cast<uint8_t*>(&pos)[j]);
                }
            }
        }
        return data;
    }

    std::vector<uint8_t> getNormalData(Matrix4f matrix = Matrix4f::identity()) {
        std::vector<uint8_t> data;
        for (auto& surface : surfaces) {
            int vertCount = surface.positionIndices.size();
            bool hasNormal = surface.hasNormal();
            for (int i = 0; i < vertCount; i++) {
                Vector3f norm;
                if (hasNormal) {
                    norm = vertexNormals[surface.normalIndices[i]];
                    norm = norm.transformPosition(matrix);
                }
                for (int j = 0; j < sizeof(Vector3f); j++) {
                    data.push_back(reinterpret_cast<uint8_t*>(&norm)[j]);
                }
            }
        }
        return data;
    }

    std::vector<uint8_t> getUVData() {
        std::vector<uint8_t> data;
        for (auto& surface : surfaces) {
            int vertCount = surface.positionIndices.size();
            bool hasUV = surface.hasUV();
            for (int i = 0; i < vertCount; i++) {
                Vector2f uv;
                if (hasUV) {
                    uv = vertexUVs[surface.uvIndices[i]];
                }
                for (int j = 0; j < sizeof(Vector2f); j++) {
                    data.push_back(reinterpret_cast<uint8_t*>(&uv)[j]);
                }
            }
        }
        return data;
    }

    std::vector<uint8_t> getData() {
        std::vector<uint8_t> data;
        for (auto& surface : surfaces) {
            int vertCount = surface.positionIndices.size();
            bool hasPosition = surface.hasPosition();
            bool hasNormal = surface.hasNormal();
            bool hasUV = surface.hasUV();
            bool hasColor = surface.hasColor();
            for (int i = 0; i < vertCount; i++) {
                Vector3f pos;
                Vector3f norm;
                Vector2f uv;

                if (hasPosition) {
                    pos = vertexPositions[surface.positionIndices[i]];
                }
                if (hasNormal) {
                    norm = vertexNormals[surface.normalIndices[i]];
                }
                if (hasUV) {
                    uv = vertexUVs[surface.uvIndices[i]];
                }

                for (int j = 0; j < sizeof(Vector3f); j++) {
                    data.push_back(reinterpret_cast<uint8_t*>(&pos)[j]);
                }
                for (int j = 0; j < sizeof(Vector3f); j++) {
                    data.push_back(reinterpret_cast<uint8_t*>(&norm)[j]);
                }
                for (int j = 0; j < sizeof(Vector2f); j++) {
                    data.push_back(reinterpret_cast<uint8_t*>(&uv)[j]);
                }
            }
        }
        return data;
    }
};

struct ModelNode : std::enable_shared_from_this<ModelNode> {

    virtual ~ModelNode();

    std::vector<ModelNode*> getAllNodes(bool includeSibling = true, bool includeChildren = true);
    Matrix4f getTransformMatrixSelf();
    Matrix4f getTransformMatrix();

    BoneID getBoneID();

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
};

struct Model {
    std::shared_ptr<ModelNode> rootNode;
    std::vector<Texture> textures;
};

struct Animation {

};

struct KeyFrame {

};

struct Sequence {

};

}
