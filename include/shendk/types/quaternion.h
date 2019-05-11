#pragma once

#include "shendk/types/vector.h"

namespace shendk {

static Vector4f quatFromEuler(Vector3f angles) {
    float rotationX = angles.z * 0.5f;
    float rotationY = angles.y * 0.5f;
    float rotationZ = angles.x * 0.5f;

    float c1 = (float)std::cos(rotationX);
    float c2 = (float)std::cos(rotationY);
    float c3 = (float)std::cos(rotationZ);
    float s1 = (float)std::sin(rotationX);
    float s2 = (float)std::sin(rotationY);
    float s3 = (float)std::sin(rotationZ);

    Vector4f quat;
    quat.w = c1 * c2 * c3 - s1 * s2 * s3;
    quat.x = s1 * c2 * c3 + c1 * s2 * s3;
    quat.y = c1 * s2 * c3 - s1 * c2 * s3;
    quat.z = c1 * c2 * s3 + s1 * s2 * c3;
    return quat;
}

static Vector4f quatFromAxis(Vector3f axis, float angle) {
    if (axis.lengthSquared() == 0.0f) {
        return Vector4f::uintW();
    }
    angle *= 0.5f;
    Vector3f normAxis = axis.normalized();
    Vector3f xyz = normAxis * std::sin(angle);
    Vector4f quat;
    quat.x = xyz.x;
    quat.y = xyz.y;
    quat.z = xyz.z;
    quat.w = std::cos(angle);
    return quat.normalized();
}

static Vector4f quatMultiply(Vector4f lhs, Vector4f rhs) {
    Vector3f xyz = lhs.xyz() * rhs.w + rhs.xyz() * lhs.w + Vector3f::cross(lhs.xyz(), rhs.xyz());
    float w = lhs.w * rhs.w - Vector3f::dot(lhs.xyz(), rhs.xyz());
    return Vector4f(xyz, w);
}

}
