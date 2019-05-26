#include "shendk/types/model.h"

#include <iostream>
#include <set>

#include "Eigen/Dense"

#include "shendk/utils/math.h"
#include "shendk/types/matrix.h"

namespace shendk {

Vertex::Vertex() {}
Vertex::Vertex(Vector3f pos, Vector3f norm) : position(pos), normal(norm) {}

Triangle::Triangle() {

}
Triangle::Triangle(Vector3f _p1, Vector3f _p2, Vector3f _p3,
                   Vector3f _n1, Vector3f _n2, Vector3f _n3)
    : p1(_p1), p2(_p2), p3(_p3)
    , n1(_n1), n2(_n2), n3(_n3)
    , bounds(_p1, _p2, _p3)
{
    center = (p1 + p2 + p3) / 3.0f;
}

Vector3f Triangle::faceNormal() {
    Vector3f normal = Vector3f::dot(p3 - p1, p2 - p1);
    if (normal.length() > 0) {
        normal.normalize();
    }
    return normal;
}

Vector3f Triangle::faceNormal(const Vector3f& _p1, const Vector3f& _p2, const Vector3f& _p3) {
    Vector3f normal = Vector3f::cross(_p2 - _p1, _p3 - _p1);
    if (normal.length() > 0) {
        normal.normalize();
    }
    return normal;
}

Vector3f Triangle::faceCenter(const Vector3f& _p1, const Vector3f& _p2, const Vector3f& _p3) {
    return (_p1 + _p2 + _p3) / 3.0f;
}

float Triangle::faceArea(const Vector3f& _p1, const Vector3f& _p2, const Vector3f& _p3) {
    float a = _p1.distance(_p2);
    float b = _p2.distance(_p3);
    float c = _p3.distance(_p1);
    float s = (a + b + c) / 2.0f;
    float A = sqrtf(s * (s-a) * (s-b) * (s-b));
    return (A > 0.0f) ? A : -A;
}

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
            int32_t newIndex = parentMesh->vertexBufferOffset + parentMesh->vertexCount + index;
            if (newIndex < 0) {
                return parentMesh->getIndex(newIndex);
            } else {
                return newIndex;
            }
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

void Model::cleanMesh(bool removeBackfaces, bool weldSimilar, bool removeUnused, float weldThreshold, float backfaceAngle) {
    const float removeThreshold = backfaceAngle / 180.0f;

    if (removeBackfaces) {
        // find backface vertices and mark them to be deleted
        std::vector<Vertex> uniqueVertices;
        std::set<uint32_t> deletedIndices;
        for (int i = 0; i < vertexBuffer.vertexCount(); i++) {
            Vector3f p = vertexBuffer.t_positions[i];
            Vector3f n = vertexBuffer.t_normals[i];
            bool exists = false;
            for (auto& v : uniqueVertices) {
                if (v.position.distance(p) < weldThreshold) {
                    float angle = v.normal.angle(n);
                    if (angle >= removeThreshold) {
                        deletedIndices.insert(i);
                        exists = true;
                    }
                }
            }
            if (!exists) {
                uniqueVertices.push_back(Vertex(p, n));
            }
        }

        // remove faces that use vertices that are assigned to be removed
        for (auto& n : rootNode->getAllNodes()) {
            if (n->mesh) {
                std::vector<MeshSurface> newSurfaces;
                for (int f = 0; f < n->mesh->surfaces.size(); f++) {
                    MeshSurface& face = n->mesh->surfaces[f];
                    MeshSurface newFace;
                    newFace.type = face.type;
                    newFace.material = face.material;
                    for (int i = 0; i < face.indexCount(); i += 3) {
                        uint32_t i1 = face.positionIndices[i];
                        uint32_t i2 = face.positionIndices[i + 1];
                        uint32_t i3 = face.positionIndices[i + 2];

                        bool addTriangle = true;
                        if (deletedIndices.find(i1) != deletedIndices.end() ||
                            deletedIndices.find(i2) != deletedIndices.end() ||
                            deletedIndices.find(i3) != deletedIndices.end()) {
                            addTriangle = false;
                        }

                        if (addTriangle) {

                            if (face.hasPosition()) {
                                newFace.positionIndices.push_back(face.positionIndices[i]);
                                newFace.positionIndices.push_back(face.positionIndices[i + 1]);
                                newFace.positionIndices.push_back(face.positionIndices[i + 2]);
                            }

                            if (face.hasNormal()) {
                                newFace.normalIndices.push_back(face.normalIndices[i]);
                                newFace.normalIndices.push_back(face.normalIndices[i + 1]);
                                newFace.normalIndices.push_back(face.normalIndices[i + 2]);
                            }

                            if (face.hasTexcoord()) {
                                newFace.texcoordIndices.push_back(face.texcoordIndices[i]);
                                newFace.texcoordIndices.push_back(face.texcoordIndices[i + 1]);
                                newFace.texcoordIndices.push_back(face.texcoordIndices[i + 2]);
                            }

                            if (face.hasColor()) {
                                newFace.colorIndices.push_back(face.colorIndices[i]);
                                newFace.colorIndices.push_back(face.colorIndices[i + 1]);
                                newFace.colorIndices.push_back(face.colorIndices[i + 2]);
                            }

                            if (face.hasJoint()) {
                                newFace.jointIndices.push_back(face.jointIndices[i]);
                                newFace.jointIndices.push_back(face.jointIndices[i + 1]);
                                newFace.jointIndices.push_back(face.jointIndices[i + 2]);
                            }

                            if (face.hasWeight()) {
                                newFace.weightIndices.push_back(face.weightIndices[i]);
                                newFace.weightIndices.push_back(face.weightIndices[i + 1]);
                                newFace.weightIndices.push_back(face.weightIndices[i + 2]);
                            }
                        }
                    }
                    if (newFace.indexCount() > 0) {
                        newSurfaces.push_back(newFace);
                    }
                }
                if (newSurfaces.size() > 0) {
                    n->mesh->surfaces = newSurfaces;
                } else {
                    delete n->mesh;
                    n->mesh = nullptr;
                }
            }
        }
    }

    if (weldSimilar) {
        // weld similar vertices
        std::vector<Vector3f> uniquePositions;
        std::vector<Vector3f> uniqueNormals;
        std::vector<Vector3f> uniqueTPositions;
        std::vector<Vector3f> uniqueTNormals;
        std::map<uint32_t, uint32_t> indexRemapping;
        for (size_t v1 = 0; v1 < vertexBuffer.vertexCount(); v1++) {
            bool weldDuplicate = false;
            for (size_t v2 = 0; v2 < uniqueTPositions.size(); v2++) {
                if (vertexBuffer.t_positions[v1].distance(uniqueTPositions[v2]) < weldThreshold) {
                    indexRemapping.insert({v1, v2});
                    weldDuplicate = true;
                    break;
                }
            }
            if (!weldDuplicate) {
                indexRemapping.insert({v1, uniquePositions.size()});
                uniquePositions.push_back(vertexBuffer.positions[v1]);
                uniqueNormals.push_back(vertexBuffer.normals[v1]);
                uniqueTPositions.push_back(vertexBuffer.t_positions[v1]);
                uniqueTNormals.push_back(vertexBuffer.t_normals[v1]);
            }
        }
        vertexBuffer.positions = uniquePositions;
        vertexBuffer.normals = uniqueNormals;
        vertexBuffer.t_positions = uniqueTPositions;
        vertexBuffer.t_normals = uniqueTNormals;
        for (auto& n : rootNode->getAllNodes()) {
            if (n->mesh) {
                for (auto& face : n->mesh->surfaces) {
                    for (int i = 0; i < face.indexCount(); i++) {
                        face.positionIndices[i] = indexRemapping[face.positionIndices[i]];
                        face.normalIndices[i] = indexRemapping[face.normalIndices[i]];
                    }
                }
            }
        }
    }

    if (removeUnused) {
        // remove unused vertex attributes
        VertexBuffer newBuffer;
        std::map<uint32_t, uint32_t> vertexIndices;
        for (auto& n : rootNode->getAllNodes()) {
            if (n->mesh) {
                for (auto& face : n->mesh->surfaces) {
                    for (int i = 0; i < face.indexCount(); i++) {

                        if (face.hasPosition()) {
                            uint32_t posIndex = face.positionIndices[i];
                            if (vertexIndices.find(posIndex) == vertexIndices.end()) {
                                face.positionIndices[i] = newBuffer.positions.size();
                                newBuffer.positions.push_back(vertexBuffer.positions[posIndex]);
                                newBuffer.t_positions.push_back(vertexBuffer.t_positions[posIndex]);
                                vertexIndices.insert({posIndex, face.positionIndices[i]});
                            } else {
                                face.positionIndices[i] = vertexIndices[posIndex];
                            }
                        }

                        if (face.hasNormal()) {
                            uint32_t normIndex = face.normalIndices[i];
                            if (vertexIndices.find(normIndex) == vertexIndices.end()) {
                                face.normalIndices[i] = newBuffer.normals.size();
                                newBuffer.normals.push_back(vertexBuffer.normals[normIndex]);
                                newBuffer.t_normals.push_back(vertexBuffer.t_normals[normIndex]);
                                vertexIndices.insert({normIndex, face.normalIndices[i]});
                            } else {
                                face.normalIndices[i] = vertexIndices[normIndex];
                            }
                        }

                        if (face.hasTexcoord()) {
                            uint32_t texIndex = face.texcoordIndices[i];
                            face.texcoordIndices[i] = newBuffer.texcoords.size();
                            newBuffer.texcoords.push_back(vertexBuffer.texcoords[texIndex]);
                        }

                        if (face.hasColor()) {
                            uint32_t colIndex = face.colorIndices[i];
                            face.colorIndices[i] = newBuffer.colors.size();
                            newBuffer.colors.push_back(vertexBuffer.colors[colIndex]);
                        }

                        if (face.hasWeight()) {
                            uint32_t weightIndex = face.weightIndices[i];
                            face.weightIndices[i] = newBuffer.weights.size();
                            newBuffer.weights.push_back(vertexBuffer.weights[weightIndex]);
                        }

                        if (face.hasJoint()) {
                            uint32_t jointIndex = face.jointIndices[i];
                            face.jointIndices[i] = newBuffer.joints.size();
                            newBuffer.joints.push_back(vertexBuffer.joints[jointIndex]);
                        }
                    }
                }
            }
        }
        vertexBuffer = newBuffer;
    }
}

}
