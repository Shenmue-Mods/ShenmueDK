#pragma once

namespace shendk {

struct Vector2f {
    float x = 0.0f;
    float y = 0.0f;

    Vector2f() {}
    Vector2f(float _x, float _y) : x(_x), y(_y) {}
};

struct Vector3f {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    Vector3f() {}
    Vector3f(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
};

struct Vector4f {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float w = 0.0f;

    Vector4f() {}
    Vector4f(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
};

}
