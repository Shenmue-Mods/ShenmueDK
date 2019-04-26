#pragma once

#include <stdint.h>
#include <vector>
#include <memory>

#include "eigen3/Eigen/Dense"
#include "eigen3/Eigen/Geometry"

#include "shendk/files/file.h"
#include "shendk/types/texture.h"
#include "shendk/utils/math.h"

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

    std::vector<ModelNode*> getAllNodes(bool includeSibling = true, bool includeChildren = true) {
        std::vector<ModelNode*> result;
        result.push_back(this);
        if (child != nullptr && includeChildren) {
            std::vector<ModelNode*> childNodes = child->getAllNodes(includeSibling, includeChildren);
            result.insert(result.end(), childNodes.begin(), childNodes.end());
        }
        if (nextSibling != nullptr && includeSibling) {
            std::vector<ModelNode*> siblingNodes = nextSibling->getAllNodes(includeSibling, includeChildren);
            result.insert(result.end(), siblingNodes.begin(), siblingNodes.end());
        }
        return result;
    }

    Eigen::Matrix4f getTransformMatrixSelf() {
        // TODO: this just hurts my eyes (clean up this mess by reading up on eigen3)
        Eigen::Vector3f xAxis(1.0f, 0.0f, 0.0f);
        Eigen::Vector3f yAxis(0.0f, 1.0f, 0.0f);
        Eigen::Vector3f zAxis(0.0f, 0.0f, 1.0f);
        Eigen::Quaternionf quatX(Eigen::AngleAxisf(rotation.x(), xAxis));
        Eigen::Quaternionf quatY(Eigen::AngleAxisf(rotation.y(), yAxis));
        Eigen::Quaternionf quatZ(Eigen::AngleAxisf(rotation.z(), zAxis));
        Eigen::Matrix4f rotMatX(quatX.toRotationMatrix());
        Eigen::Matrix4f rotMatY(quatY.toRotationMatrix());
        Eigen::Matrix4f rotMatZ(quatZ.toRotationMatrix());
        Eigen::Matrix4f scaleMatrix(Eigen::Matrix4f::Identity());
        scaleMatrix.row(0)[0] = scale.x();
        scaleMatrix.row(1)[1] = scale.y();
        scaleMatrix.row(2)[2] = scale.z();
        Eigen::Matrix4f translationMatrix(Eigen::Matrix4f::Identity());
        translationMatrix.row(3)[0] = position.x();
        translationMatrix.row(3)[1] = position.y();
        translationMatrix.row(3)[2] = position.z();
        return scaleMatrix * rotMatX * rotMatY * rotMatZ * translationMatrix;
    }

    Eigen::Matrix4f getTransformMatrix() {
        Eigen::Matrix4f matrix = Eigen::Matrix4f::Identity();
        if (parent != nullptr)
        {
            matrix = parent->getTransformMatrix();
        }
        return getTransformMatrixSelf() * matrix;
    }

    virtual ~ModelNode() {}

    uint32_t id;

    Eigen::Vector3f position;
    Eigen::Vector3f rotation;
    Eigen::Vector3f scale;
    Eigen::Vector3f center;
    float radius;

    ModelNode* child;
    ModelNode* nextSibling;
    ModelNode* parent;

    std::vector<Eigen::Vector3f> vertexPositions;
    std::vector<Eigen::Vector3f> vertexNormals;
    std::vector<Eigen::Vector2f> vertexUVs;
    std::vector<Eigen::Vector4f> vertexColors;
    std::vector<MeshSurface> surfaces;

};

struct Model {

    // Model ID = {0x00;LayerID;TypeID;BoneID}



    ModelNode* rootNode;
    std::vector<std::shared_ptr<Texture>> textures;
};



}
