#include "shendk/types/bounding_box.h"

#include <algorithm>

namespace shendk {

AABoundingBox::AABoundingBox() {}
AABoundingBox::AABoundingBox(const Vector3f& _min, const Vector3f& _max) : min(_min), max(_max) {}
AABoundingBox::AABoundingBox(const Vector3f& _p1, const Vector3f& _p2, const Vector3f& _p3) {
    min.x = std::min(std::min(_p1.x, _p2.x), _p3.x);
    min.y = std::min(std::min(_p1.y, _p2.y), _p3.y);
    min.z = std::min(std::min(_p1.z, _p2.z), _p3.z);
    max.x = std::max(std::max(_p1.x, _p2.x), _p3.x);
    max.y = std::max(std::max(_p1.y, _p2.y), _p3.y);
    max.z = std::max(std::max(_p1.z, _p2.z), _p3.z);
}

bool AABoundingBox::inside(const Vector3f& v, float threshold) {
    return ((v.x + threshold) >= min.x && (v.x - threshold) <= max.x) &&
           ((v.y + threshold) >= min.y && (v.y - threshold) <= max.y) &&
           ((v.z + threshold) >= min.z && (v.z - threshold) <= max.z);
}

bool AABoundingBox::inside(const AABoundingBox& box, float threshold) {
    return ((box.min.x + threshold) >= min.x && (box.max.x - threshold) <= max.x) &&
           ((box.min.y + threshold) >= min.y && (box.max.y - threshold) <= max.y) &&
           ((box.min.z + threshold) >= min.z && (box.max.z - threshold) <= max.z);
}

}
