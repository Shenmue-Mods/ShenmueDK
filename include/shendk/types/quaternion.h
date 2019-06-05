#pragma once

#include "shendk/types/vector.h"

namespace shendk {

static Vector4f quatFromEuler(const Vector3f& angles) {
    float rotationX = angles.z * 0.5f;
    float rotationY = angles.y * 0.5f;
    float rotationZ = angles.x * 0.5f;

    float c1 = std::cos(rotationX);
    float c2 = std::cos(rotationY);
    float c3 = std::cos(rotationZ);
    float s1 = std::sin(rotationX);
    float s2 = std::sin(rotationY);
    float s3 = std::sin(rotationZ);

    Vector4f quat;
    quat.w = c1 * c2 * c3 - s1 * s2 * s3;
    quat.x = s1 * c2 * c3 + c1 * s2 * s3;
    quat.y = c1 * s2 * c3 - s1 * c2 * s3;
    quat.z = c1 * c2 * s3 + s1 * s2 * c3;
    return quat;
}

static Vector4f quatFromAxis(const Vector3f& axis, float angle) {
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

static Vector4f quatMultiply(const Vector4f& lhs, const Vector4f& rhs) {
    Vector3f xyz = lhs.xyz() * rhs.w + rhs.xyz() * lhs.w + Vector3f::cross(lhs.xyz(), rhs.xyz());
    float w = lhs.w * rhs.w - Vector3f::dot(lhs.xyz(), rhs.xyz());
    return Vector4f(xyz, w);
}

static Vector4f axisAngleFromQuat(const Vector4f& quat) {
    Vector4f q = quat;
    if (std::abs(q.w) > 1.0f) {
        q.normalize();
    }

    Vector4f result;
    result.w = 2.0f * std::acos(q.w);
    float den = std::sqrt(1.0f - q.w * q.w);
    if (den > 0.0001f) {
        result.x = q.x / den;
        result.y = q.y / den;
        result.z = q.z / den;
    } else {
        result.x = 1.0f;
        result.y = 0.0f;
        result.z = 0.0f;
    }
    return result;
}

}
