#include "shendk/types/model.h"

#include "Eigen/Dense"
#include "shendk/utils/math.h"
#include "shendk/types/matrix.h"
#include <iostream>

namespace shendk {

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
    return static_cast<BoneID>(id & 0xFF);
}

}
