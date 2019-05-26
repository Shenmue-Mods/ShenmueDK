#pragma once

#include <math.h>
#include <stdint.h>
#define M_PI       3.14159265358979323846
#define M_PIf      3.14159265358979323846f

namespace shendk {

inline float degreesToRadians(float degrees) {
    return degrees * (M_PIf / 180.0f);
}

inline double degreesToRadians(double degrees) {
    return degrees * (M_PI / 180.0);
}

inline float radiansToDegrees(float radians) {
    return radians * (180.0f / M_PIf);
}

inline double radiansToDegrees(double radians) {
    return radians * (180.0 / M_PI);
}

inline float ushortToDegrees(uint16_t value) {
    return static_cast<float>(value) / 65535.0f * 360.0f;
}

inline uint16_t degreesToUshort(float degrees) {
    return static_cast<uint16_t>(degrees / 360.0f * 65535.0f);
}

inline static float inverseSqrtFast(float x) {
    float xhalf = 0.5f * x;
    int i = *reinterpret_cast<int*>(&x);
    i = 0x5f375a86 - (i >> 1);
    x = *reinterpret_cast<float*>(&i);
    x = x * (1.5f - xhalf * x * x);
    return x;
}

inline float clamp(float value, float min, float max) {
    return std::max(std::min(value, max), min);
}

inline double clamp(double value, double min, double max) {
    return std::max(std::min(value, max), min);
}


}
