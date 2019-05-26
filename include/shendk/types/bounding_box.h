#pragma once

#include "shendk/types/vector.h"

namespace shendk {

/**
 * @brief Axis-aligned bounding box (AABB)
 */
struct AABoundingBox {
    Vector3f min;
    Vector3f max;

    AABoundingBox();
    AABoundingBox(const Vector3f& _min, const Vector3f& _max);
    AABoundingBox(const Vector3f& _p1, const Vector3f& _p2, const Vector3f& _p3);

    bool inside(const Vector3f& v, float threshold = 0.00001f);
    bool inside(const AABoundingBox& box, float threshold = 0.00001f);
};

}
