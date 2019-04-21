#pragma once

#include <math.h>

namespace shendk {

static float degreesToRadians(float degrees) {
    const float degToRad = M_PI / 180.0f;
    return degrees * degToRad;
}

static double degreesToRadians(double degrees) {
    const double degToRad = M_PI / 180.0;
    return degrees * degToRad;
}

static float radiansToDegrees(float radians) {
    const float radToDeg = 180.0f / M_PI;
    return radians * radToDeg;
}

static double radiansToDegrees(double radians) {
    const double radToDeg = 180.0 / M_PI;
    return radians * radToDeg;
}

}
