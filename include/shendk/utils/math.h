#pragma once

#include <math.h>
#include <stdint.h>

namespace shendk {

inline float degreesToRadians(float degrees) {
    const float degToRad = M_PI / 180.0f;
    return degrees * degToRad;
}

inline double degreesToRadians(double degrees) {
    const double degToRad = M_PI / 180.0;
    return degrees * degToRad;
}

inline float radiansToDegrees(float radians) {
    const float radToDeg = 180.0f / M_PI;
    return radians * radToDeg;
}

inline double radiansToDegrees(double radians) {
    const double radToDeg = 180.0 / M_PI;
    return radians * radToDeg;
}

inline float ushortToDegrees(uint16_t value) {
    return  value / 0xFFFF * 360.0f;
}

inline uint16_t degreesToUshort(float degrees) {
    return static_cast<uint16_t>(degrees / 360.0f * 0xffff);
}

}
