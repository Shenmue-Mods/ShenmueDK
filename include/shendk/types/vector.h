#pragma once

#include <stdexcept>
#include <math.h>
#include <vector>

namespace shendk {

struct Vector2f;
struct Vector3f;
struct Vector4f;
struct Matrix4f;

struct Vector2f {
    float x = 0.0f;
    float y = 0.0f;

    Vector2f();
    Vector2f(float value);
    Vector2f(float _x, float _y);

    std::string str();
};

struct Vector3f {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    Vector3f();
    Vector3f(Vector2f v);
    Vector3f(Vector4f& v);
    Vector3f(float value);
    Vector3f(float _x, float _y);
    Vector3f(float _x, float _y, float _z);

    std::string str();

    float operator[](int index) const;

    Vector3f operator-();

    Vector3f operator+(float value);
    Vector3f operator+(const Vector3f& rhs);
    Vector3f operator+=(float value);
    Vector3f operator+=(const Vector3f& rhs);

    Vector3f operator-(float value);
    Vector3f operator-(const Vector3f& rhs);
    Vector3f operator-=(float value);
    Vector3f operator-=(const Vector3f& rhs);

    Vector3f operator*(float value);
    Vector3f operator*(const Vector3f& rhs);
    Vector3f operator*=(float value);
    Vector3f operator*=(const Vector3f& rhs);

    Vector3f operator/(float value);
    Vector3f operator/(const Vector3f& rhs);
    Vector3f operator/=(float value);
    Vector3f operator/=(const Vector3f& rhs);

    float length();
    float lengthSquared();

    void normalize();
    Vector3f normalized();

    float dot(const Vector3f& rhs);
    static float dot(const Vector3f& lhs, const Vector3f& rhs);

    Vector3f cross(const Vector3f& rhs);
    static Vector3f cross(const Vector3f& lhs, const Vector3f& rhs);
    static void cross(const Vector3f& lhs, const Vector3f& rhs, Vector3f& out);

    Vector3f transformPosition(const Matrix4f& rhs);
    static Vector3f transformPosition(const Vector3f& lhs, const Matrix4f& rhs);
    static void transformPosition(const Vector3f& lhs, const Matrix4f& rhs, Vector3f& out);

    static Vector3f uintX();
    static Vector3f uintY();
    static Vector3f uintZ();
    static Vector3f zero();
    static Vector3f one();
};

struct Vector4f {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float w = 0.0f;

    Vector4f();
    Vector4f(Vector2f v);
    Vector4f(Vector3f v);
    Vector4f(Vector3f v, float w);
    Vector4f(float value);
    Vector4f(float _x, float _y);
    Vector4f(float _x, float _y, float _z);
    Vector4f(float _x, float _y, float _z, float _w);

    std::string str();

    float operator[](int index) const;

    Vector4f operator-();

    Vector4f operator+(float value);
    Vector4f operator+(const Vector4f& rhs);
    Vector4f operator+=(float value);
    Vector4f operator+=(const Vector4f& rhs);

    Vector4f operator-(float value);
    Vector4f operator-(const Vector4f& rhs);
    Vector4f operator-=(float value);
    Vector4f operator-=(const Vector4f& rhs);

    Vector4f operator*(float value);
    Vector4f operator*(const Vector4f& rhs);
    Vector4f operator*=(float value);
    Vector4f operator*=(const Vector4f& rhs);

    Vector4f operator/(float value);
    Vector4f operator/(const Vector4f& rhs);
    Vector4f operator/=(float value);
    Vector4f operator/=(const Vector4f& rhs);

    Vector3f xyz();

    float length();
    float lengthSquared();

    void normalize();
    Vector4f normalized();

    float dot(const Vector4f& rhs);
    static float dot(const Vector4f& lhs, const Vector4f& rhs);

    Vector4f transform(const Matrix4f& rhs);
    static Vector4f transform(const Vector4f& lhs, const Matrix4f& rhs);
    static void transform(const Vector4f& lhs, const Matrix4f& rhs, Vector4f& out);

    static Vector4f uintX();
    static Vector4f uintY();
    static Vector4f uintZ();
    static Vector4f uintW();
    static Vector4f zero();
    static Vector4f one();
};

}
