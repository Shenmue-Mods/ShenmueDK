#include "shendk/types/model.h"

#include "Eigen/Dense"
#include "shendk/utils/math.h"
#include "shendk/types/matrix.h"
#include <iostream>

namespace shendk {


std::vector<uint8_t> VertexBuffer::getPositionData(Matrix4f matrix) {
    std::vector<uint8_t> data;
    for (auto& vertex : positions) {
        Vector3f pos = vertex;
        pos = pos.transformPosition(matrix);
        for (size_t j = 0; j < sizeof(Vector3f); j++) {
            data.push_back(reinterpret_cast<uint8_t*>(&pos)[j]);
        }
    }
    return data;
}

std::vector<uint8_t> VertexBuffer::getNormalData(Matrix4f matrix) {
    std::vector<uint8_t> data;
    for (auto& vertex : normals) {
        Vector3f norm = vertex;
        norm = norm.transformPosition(matrix);
        for (size_t j = 0; j < sizeof(Vector3f); j++) {
            data.push_back(reinterpret_cast<uint8_t*>(&norm)[j]);
        }
    }
    return data;
}

std::vector<uint8_t> VertexBuffer::getTexcoordData() {
    std::vector<uint8_t> data;
    for (auto& vertex : texcoords) {
        Vector2f texcoord = vertex;
        for (size_t j = 0; j < sizeof(Vector2f); j++) {
            data.push_back(reinterpret_cast<uint8_t*>(&texcoord)[j]);
        }
    }
    return data;
}

std::vector<uint8_t> VertexBuffer::getColorData() {
    std::vector<uint8_t> data;
    for (auto& vertex : colors) {
        Vector4f color = vertex;
        for (size_t j = 0; j < sizeof(Vector4f); j++) {
            data.push_back(reinterpret_cast<uint8_t*>(&color)[j]);
        }
    }
    return data;
}

std::vector<uint8_t> VertexBuffer::getWeightsData() {
    std::vector<uint8_t> data;
    for (auto& vertex : weights) {
        Vector4f weight = Vector4f(vertex);
        for (size_t j = 0; j < sizeof(Vector4f); j++) {
            data.push_back(reinterpret_cast<uint8_t*>(&weight)[j]);
        }
    }
    return data;
}

std::vector<uint8_t> VertexBuffer::getJointsData() {
    // TODO: need to assign node index to bone id
    /*std::vector<uint8_t> data;
    for (auto& vertex : joints) {
        Vector4i16 joint = Vector4i16(vertex);
        for (int j = 0; j < sizeof(Vector4i16); j++) {
            data.push_back(reinterpret_cast<uint8_t*>(&joint)[j]);
        }
    }
    return data;*/
	return std::vector<uint8_t>();
}

void MeshSurface::mergeSurface(MeshSurface& rhs) {
    if (type != PrimitiveType::Triangles) {
        throw new std::runtime_error("Only triangle surfaces can be merged.");
    }
    if (hasPosition()) {
        positionIndices.insert(positionIndices.end(), rhs.positionIndices.begin(), rhs.positionIndices.end());
    }
    if (hasNormal()) {
        normalIndices.insert(normalIndices.end(), rhs.normalIndices.begin(), rhs.normalIndices.end());
    }
    if (hasTexcoord()) {
        texcoordIndices.insert(texcoordIndices.end(), rhs.texcoordIndices.begin(), rhs.texcoordIndices.end());
    }
    if (hasColor()) {
        colorIndices.insert(colorIndices.end(), rhs.colorIndices.begin(), rhs.colorIndices.end());
    }
}

void MeshSurface::convertToTriangles() {
    int32_t count = indexCount();
    if (hasPosition()) {
        std::vector<uint32_t> newIndices;
        for (size_t i = 0; i < static_cast<size_t>(count - 2); i++) {
            uint32_t index1 = positionIndices[i];
            uint32_t index2 = positionIndices[i + 1];
            uint32_t index3 = positionIndices[i + 2];
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
        std::vector<uint32_t> newIndices;
        for (size_t i = 0; i < static_cast<size_t>(count - 2); i++) {
            uint32_t index1 = normalIndices[i];
            uint32_t index2 = normalIndices[i + 1];
            uint32_t index3 = normalIndices[i + 2];
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
    if (hasTexcoord()){
        std::vector<uint32_t> newIndices;
        for (size_t i = 0; i < static_cast<size_t>(count - 2); i++) {
            uint32_t index1 = texcoordIndices[i];
            uint32_t index2 = texcoordIndices[i + 1];
            uint32_t index3 = texcoordIndices[i + 2];
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
        texcoordIndices = newIndices;
    }
    if (hasColor()){
        std::vector<uint32_t> newIndices;
        for (size_t i = 0; i < static_cast<size_t>(count - 2); i++) {
            uint32_t index1 = colorIndices[i];
            uint32_t index2 = colorIndices[i + 1];
            uint32_t index3 = colorIndices[i + 2];
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
    if (hasWeight()){
        std::vector<uint32_t> newIndices;
        for (size_t i = 0; i < static_cast<size_t>(count - 2); i++) {
            uint32_t index1 = weightIndices[i];
            uint32_t index2 = weightIndices[i + 1];
            uint32_t index3 = weightIndices[i + 2];
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
        weightIndices = newIndices;
    }
    if (hasJoint()){
        std::vector<uint32_t> newIndices;
        for (size_t i = 0; i < static_cast<size_t>(count - 2); i++) {
            uint32_t index1 = jointIndices[i];
            uint32_t index2 = jointIndices[i + 1];
            uint32_t index3 = jointIndices[i + 2];
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
        jointIndices = newIndices;
    }
    type = PrimitiveType::Triangles;
}

NodeMesh::NodeMesh(ModelNode* _node)
    : node(_node)
{
    vertexBufferOffset = node->model->vertexBuffer.vertexCount();
}

VertexBuffer& NodeMesh::vertexBuffer() {
    return node->model->vertexBuffer;
}

uint32_t NodeMesh::getIndex(int32_t index) {
    if (index < 0) {
        NodeMesh* parentMesh = node->parent->mesh;
        if (parentMesh) {
            return parentMesh->vertexBufferOffset + parentMesh->vertexCount + index;
        }
    } else {
        return vertexBufferOffset + index;
    }
    return 0;
}

ModelNode::ModelNode(Model* _model)
    : model(_model)
{
}

ModelNode::~ModelNode() {
    if (child) { delete child; }
    if (nextSibling) { delete nextSibling; }
    if (mesh) { delete mesh; }
}

std::vector<ModelNode*> ModelNode::getAllNodes(bool includeSibling, bool includeChildren) {
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

Matrix4f ModelNode::getTransformMatrixSelf() {
    Matrix4f rotationMatrixX = Matrix4f::createFromAxisAngle(Vector3f::uintX(), degreesToRadians(rotation.x));
    Matrix4f rotationMatrixY = Matrix4f::createFromAxisAngle(Vector3f::uintY(), degreesToRadians(rotation.y));
    Matrix4f rotationMatrixZ = Matrix4f::createFromAxisAngle(Vector3f::uintZ(), degreesToRadians(rotation.z));
    Matrix4f scaleMatrix = Matrix4f::createScale(scale.x, scale.y, scale.z);
    Matrix4f translateMatrix = Matrix4f::createTranslation(position.x, position.y, position.z);
    return scaleMatrix * rotationMatrixX * rotationMatrixY * rotationMatrixZ * translateMatrix;
}

Matrix4f ModelNode::getTransformMatrix() {
    Matrix4f matrix = Matrix4f::identity();
    if (parent) {
        matrix = parent->getTransformMatrix();
    }
    return getTransformMatrixSelf() * matrix;
}

BoneID ModelNode::getBoneID() {
    if (mesh) {
        return static_cast<BoneID>(id & 0xFF);
    }
    return BoneID::None;
}

}
