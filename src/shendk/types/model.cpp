#include "shendk/types/model.h"

#include "Eigen/Dense"
#include "shendk/utils/math.h"

namespace shendk {

ModelNode::~ModelNode() {}

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

Matrix4x4f ModelNode::getTransformMatrixSelf() {
    /*Eigen::Vector3f ePos(position.x, position.y, position.z);
    Eigen::Vector3f eScl(scale.x, scale.y, scale.z);
    Eigen::Quaternionf quatX(Eigen::AngleAxisf(rotation.x, Eigen::Vector3f::UnitX()));
    Eigen::Quaternionf quatY(Eigen::AngleAxisf(rotation.y, Eigen::Vector3f::UnitY()));
    Eigen::Quaternionf quatZ(Eigen::AngleAxisf(rotation.z, Eigen::Vector3f::UnitZ()));
    Eigen::Matrix4f rotMatX;// = quatX.matrix(); // TODO: ...
    Eigen::Matrix4f rotMatY;// = quatY.matrix();
    Eigen::Matrix4f rotMatZ;// = quatZ.matrix();
    Eigen::Matrix4f scaleMatrix(Eigen::Matrix4f::Identity());
    scaleMatrix.row(0)[0] = eScl.x();
    scaleMatrix.row(1)[1] = eScl.y();
    scaleMatrix.row(2)[2] = eScl.z();
    Eigen::Matrix4f translationMatrix(Eigen::Matrix4f::Identity());
    translationMatrix.row(3)[0] = ePos.x();
    translationMatrix.row(3)[1] = ePos.y();
    translationMatrix.row(3)[2] = ePos.z();
    Eigen::Translation3f translation(ePos);*/
    Matrix4x4f matrix;
    return matrix;
}

Matrix4x4f ModelNode::getTransformMatrix() {
    /*Eigen::Transform<float,3,Eigen::Affine> matrix;
    if (parent != nullptr)
    {
        matrix = parent->getTransformMatrix();
    }*/
    Matrix4x4f mat;
    return mat;
}

}
