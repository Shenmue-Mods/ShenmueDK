#include "shendk/types/vector.h"

#include "shendk/types/matrix.h"
#include "shendk/utils/math.h"

#include <stdexcept>
#include <math.h>
#include <sstream>

namespace shendk {

Vector2f::Vector2f() {}
Vector2f::Vector2f(float _x, float _y) : x(_x), y(_y) {}

std::string Vector2f::str() {
    std::stringstream ss;
    ss << "{" << x << ", " << y << "}";
    return ss.str();
}


Vector3f::Vector3f() : x(0.0f), y(0.0f), z(0.0f) {}
Vector3f::Vector3f(Vector2f v) : x(v.x), y(v.y), z(0.0f) {}
Vector3f::Vector3f(Vector4f& v) : x(v.x), y(v.y), z(v.z) {}
Vector3f::Vector3f(float value) : x(value), y(value), z(value) {}
Vector3f::Vector3f(float _x, float _y) : x(_x), y(_y), z(0.0f) {}
Vector3f::Vector3f(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}

std::string Vector3f::str() {
    std::stringstream ss;
    ss << "{" << x << ", " << y << ", " << z << "}";
    return ss.str();
}

float Vector3f::operator[](int index) const {
    switch(index) {
        case 0:
            return x;
        case 1:
            return y;
        case 2:
            return z;
        default:
            throw new std::runtime_error("Index out of range!");
    }
}

Vector3f Vector3f::operator-() {
    return Vector3f(-x, -y, -z);
}

Vector3f Vector3f::operator+(float value) const {
    Vector3f v(*this);
    v += value;
    return v;
}
Vector3f Vector3f::operator+(const Vector3f& rhs) const {
    Vector3f v(*this);
    v += rhs;
    return v;
}
Vector3f Vector3f::operator+=(float value) {
    x += value; y += value; z += value;
    return *this;
}
Vector3f Vector3f::operator+=(const Vector3f& rhs) {
    x += rhs.x; y += rhs.y; z += rhs.z;
    return *this;
}

Vector3f Vector3f::operator-(float value) const {
    Vector3f v(*this);
    v -= value;
    return v;
}
Vector3f Vector3f::operator-(const Vector3f& rhs) const {
    Vector3f v(*this);
    v -= rhs;
    return v;
}
Vector3f Vector3f::operator-=(float value) {
    x -= value; y -= value; z -= value;
    return *this;
}
Vector3f Vector3f::operator-=(const Vector3f& rhs) {
    x -= rhs.x; y -= rhs.y; z -= rhs.z;
    return *this;
}

Vector3f Vector3f::operator*(float value) const {
    Vector3f v(*this);
    v *= value;
    return v;
}
Vector3f Vector3f::operator*(const Vector3f& rhs) const {
    Vector3f v(*this);
    v *= rhs;
    return v;
}
Vector3f Vector3f::operator*=(float value) {
    x *= value; y *= value; z *= value;
    return *this;
}
Vector3f Vector3f::operator*=(const Vector3f& rhs) {
    x *= rhs.x; y *= rhs.y; z *= rhs.z;
    return *this;
}

Vector3f Vector3f::operator/(float value) const {
    Vector3f v(*this);
    v /= value;
    return v;
}
Vector3f Vector3f::operator/(const Vector3f& rhs) const {
    Vector3f v(*this);
    v /= rhs;
    return v;
}
Vector3f Vector3f::operator/=(float value) {
    x /= value; y /= value; z /= value;
    return *this;
}
Vector3f Vector3f::operator/=(const Vector3f& rhs) {
    x /= rhs.x; y /= rhs.y; z /= rhs.z;
    return *this;
}

bool Vector3f::operator==(const Vector3f& rhs) {
    return (x == rhs.x && y == rhs.y && z == rhs.z);
}

Vector3f Vector3f::abs() { return abs(*this); }
Vector3f Vector3f::abs(const Vector3f& v) {
    Vector3f result(v);
    result.x = std::abs(v.x);
    result.y = std::abs(v.y);
    result.z = std::abs(v.z);
    return result;
}

float Vector3f::length() const { return std::sqrt(x * x + y * y + z * z); }
float Vector3f::lengthSquared() const { return x * x + y * y + z * z; }

void Vector3f::normalize() {
    float scale = 1.0f / length();
    x *= scale;
    y *= scale;
    z *= scale;
}
Vector3f Vector3f::normalized() {
    Vector3f v(*this);
    v.normalize();
    return v;
}

float Vector3f::dot(const Vector3f& rhs) { return dot(*this, rhs); }
float Vector3f::dot(const Vector3f& lhs, const Vector3f& rhs) {
    return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

Vector3f Vector3f::cross(const Vector3f& rhs) { return cross(*this, rhs); }
Vector3f Vector3f::cross(const Vector3f& lhs, const Vector3f& rhs) {
    Vector3f v;
    cross(lhs, rhs, v);
    return v;
}
void Vector3f::cross(const Vector3f& lhs, const Vector3f& rhs, Vector3f& out) {
    out.x = lhs.y * rhs.z - lhs.z * rhs.y;
    out.y = lhs.z * rhs.x - lhs.x * rhs.z;
    out.z = lhs.x * rhs.y - lhs.y * rhs.x;
}

float Vector3f::angle(const Vector3f& rhs) { return angle(*this, rhs); }
float Vector3f::angle(const Vector3f& lhs, const Vector3f& rhs) {
    return std::acos(clamp(dot(lhs, rhs) / (lhs.length() * rhs.length()), -1.0f, 1.0f));
}

bool Vector3f::compareDir(const Vector3f& rhs) { return compareDir(*this, rhs); }
bool Vector3f::compareDir(const Vector3f& lhs, const Vector3f& rhs) {
    return ((lhs.x > 0 && rhs.x > 0) || (lhs.x < 0 && rhs.x < 0) || (lhs.x == 0.0f && rhs.x == 0.0f)) &&
           ((lhs.y > 0 && rhs.y > 0) || (lhs.y < 0 && rhs.y < 0) || (lhs.y == 0.0f && rhs.y == 0.0f)) &&
           ((lhs.z > 0 && rhs.z > 0) || (lhs.z < 0 && rhs.z < 0) || (lhs.z == 0.0f && rhs.z == 0.0f));
}

float Vector3f::distance(const Vector3f& rhs) const { return distance(*this, rhs); }
float Vector3f::distance(const Vector3f& lhs, const Vector3f& rhs) {
    float x = lhs.x - rhs.x;
    float y = lhs.y - rhs.y;
    float z = lhs.z - rhs.z;
    float dist;
    dist = x * x + y * y + z * z;
    dist = sqrt(dist);
    return dist;
}

Vector3f Vector3f::transformPosition(const Matrix4f& rhs) { return transformPosition(*this, rhs); }
Vector3f Vector3f::transformPosition(const Vector3f& lhs, const Matrix4f& rhs) {
    Vector3f v;
    transformPosition(lhs, rhs, v);
    return v;
}
void Vector3f::transformPosition(const Vector3f& lhs, const Matrix4f& rhs, Vector3f& out) {
    out.x = lhs.x * rhs.row0.x +
            lhs.y * rhs.row1.x +
            lhs.z * rhs.row2.x +
            rhs.row3.x;
    out.y = lhs.x * rhs.row0.y +
            lhs.y * rhs.row1.y +
            lhs.z * rhs.row2.y +
            rhs.row3.y;
    out.z = lhs.x * rhs.row0.z +
            lhs.y * rhs.row1.z +
            lhs.z * rhs.row2.z +
            rhs.row3.z;
}

Vector3f Vector3f::uintX() { return Vector3f(1.0f, 0.0f, 0.0f); }
Vector3f Vector3f::uintY() { return Vector3f(0.0f, 1.0f, 0.0f); }
Vector3f Vector3f::uintZ() { return Vector3f(0.0f, 0.0f, 1.0f); }
Vector3f Vector3f::zero() { return Vector3f(0.0f, 0.0f, 0.0f); }
Vector3f Vector3f::one() { return Vector3f(1.0f, 1.0f, 1.0f); }


Vector4f::Vector4f() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
Vector4f::Vector4f(Vector2f v) : x(v.x), y(v.y), z(0.0f), w(0.0f) {}
Vector4f::Vector4f(Vector3f v) : x(v.x), y(v.y), z(v.z), w(0.0f) {}
Vector4f::Vector4f(Vector3f v, float w) : x(v.x), y(v.y), z(v.z), w(w) {}
Vector4f::Vector4f(float value) : x(value), y(value), z(value), w(value) {}
Vector4f::Vector4f(float _x, float _y) : x(_x), y(_y), z(0.0f), w(0.0f) {}
Vector4f::Vector4f(float _x, float _y, float _z) : x(_x), y(_y), z(_z), w(0.0f) {}
Vector4f::Vector4f(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}

std::string Vector4f::str() {
    std::stringstream ss;
    ss << "{" << x << ", " << y << ", " << z << ", " << w << "}";
    return ss.str();
}

float Vector4f::operator[](int index) const {
    switch(index) {
    case 0:
        return x;
    case 1:
        return y;
    case 2:
        return z;
    case 3:
        return w;
    default:
        throw new std::runtime_error("Index out of range!");
    }
}

Vector4f Vector4f::operator-() {
    return Vector4f(-x, -y, -z, -w);
}

Vector4f Vector4f::operator+(float value) {
    Vector4f v(*this);
    v += value;
    return v;
}
Vector4f Vector4f::operator+(const Vector4f& rhs) {
    Vector4f v(*this);
    v += rhs;
    return v;
}
Vector4f Vector4f::operator+=(float value) {
    x += value; y += value; z += value; w += value;
    return *this;
}
Vector4f Vector4f::operator+=(const Vector4f& rhs) {
    x += rhs.x; y += rhs.y; z += rhs.z; w += rhs.w;
    return *this;
}

Vector4f Vector4f::operator-(float value) {
    Vector4f v(*this);
    v -= value;
    return v;
}
Vector4f Vector4f::operator-(const Vector4f& rhs) {
    Vector4f v(*this);
    v -= rhs;
    return v;
}
Vector4f Vector4f::operator-=(float value) {
    x -= value; y -= value; z -= value; w -= value;
    return *this;
}
Vector4f Vector4f::operator-=(const Vector4f& rhs) {
    x -= rhs.x; y -= rhs.y; z -= rhs.z; w -= rhs.w;
    return *this;
}

Vector4f Vector4f::operator*(float value) {
    Vector4f v(*this);
    v *= value;
    return v;
}
Vector4f Vector4f::operator*(const Vector4f& rhs) {
    Vector4f v(*this);
    v *= rhs;
    return v;
}
Vector4f Vector4f::operator*=(float value) {
    x *= value; y *= value; z *= value; w *= value;
    return *this;
}
Vector4f Vector4f::operator*=(const Vector4f& rhs) {
    x *= rhs.x; y *= rhs.y; z *= rhs.z; w *= rhs.w;
    return *this;
}

Vector4f Vector4f::operator/(float value) {
    Vector4f v(*this);
    v /= value;
    return v;
}
Vector4f Vector4f::operator/(const Vector4f& rhs) {
    Vector4f v(*this);
    v /= rhs;
    return v;
}
Vector4f Vector4f::operator/=(float value) {
    x /= value; y /= value; z /= value; w /= value;
    return *this;
}
Vector4f Vector4f::operator/=(const Vector4f& rhs) {
    x /= rhs.x; y /= rhs.y; z /= rhs.z; w /= rhs.w;
    return *this;
}

Vector3f Vector4f::xyz() {
    return Vector3f(x, y, z);
}

float Vector4f::length() { return std::sqrt(x * x + y * y + z * z + w * w); }
float Vector4f::lengthSquared() { return x * x + y * y + z * z + w * w; }

void Vector4f::normalize() {
    float scale = 1.0f / length();
    x *= scale;
    y *= scale;
    z *= scale;
    w *= scale;
}
Vector4f Vector4f::normalized() {
    Vector4f v(*this);
    v.normalize();
    return v;
}

float Vector4f::dot(const Vector4f& rhs) { return dot(*this, rhs); }
float Vector4f::dot(const Vector4f& lhs, const Vector4f& rhs) {
    return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z + lhs.w * rhs.w;
}

Vector4f Vector4f::transform(const Matrix4f& rhs) { return transform(*this, rhs); }
Vector4f Vector4f::transform(const Vector4f& lhs, const Matrix4f& rhs) {
    Vector4f v;
    transform(lhs, rhs, v);
    return v;
}
void Vector4f::transform(const Vector4f& lhs, const Matrix4f& rhs, Vector4f& out) {
    out.x = lhs.x * rhs.row0.x + lhs.y * rhs.row1.x + lhs.z * rhs.row2.x + lhs.w * rhs.row3.x;
    out.y = lhs.x * rhs.row0.y + lhs.y * rhs.row1.y + lhs.z * rhs.row2.y + lhs.w * rhs.row3.y;
    out.z = lhs.x * rhs.row0.z + lhs.y * rhs.row1.z + lhs.z * rhs.row2.z + lhs.w * rhs.row3.z;
    out.w = lhs.x * rhs.row0.w + lhs.y * rhs.row1.w + lhs.z * rhs.row2.w + lhs.w * rhs.row3.w;
}

Vector4f Vector4f::uintX() { return Vector4f(1.0f, 0.0f, 0.0f, 0.0f); }
Vector4f Vector4f::uintY() { return Vector4f(0.0f, 1.0f, 0.0f, 0.0f); }
Vector4f Vector4f::uintZ() { return Vector4f(0.0f, 0.0f, 1.0f, 0.0f); }
Vector4f Vector4f::uintW() { return Vector4f(0.0f, 0.0f, 0.0f, 1.0f); }
Vector4f Vector4f::zero() { return Vector4f(0.0f, 0.0f, 0.0f, 0.0f); }
Vector4f Vector4f::one() { return Vector4f(1.0f, 1.0f, 1.0f, 1.0f); }

Vector4i16::Vector4i16(uint16_t value)
    : x(value), y(value), z(value), w(value)
{}

}
