#pragma once

#include <math.h>
#include <stdint.h>
#define M_PI       3.14159265358979323846

namespace shendk {

inline float degreesToRadians(float degrees) {
    return degrees * M_PI / 180.0f;
}

inline double degreesToRadians(double degrees) {
    return degrees * M_PI / 180.0;
}

inline float radiansToDegrees(float radians) {
    return radians * 180.0f / M_PI;
}

inline double radiansToDegrees(double radians) {
    return radians * 180.0 / M_PI;
}

inline float ushortToDegrees(uint16_t value) {
    return  value / 0xFFFF * 360.0f;
}

inline uint16_t degreesToUshort(float degrees) {
    return static_cast<uint16_t>(degrees / 360.0f * 0xffff);
}

}
