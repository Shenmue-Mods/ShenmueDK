#include "shendk/types/matrix.h"

#include <stdexcept>
#include <math.h>
#include <sstream>
#include <iomanip>

namespace shendk {

Matrix4f::Matrix4f()
    : row0(Vector4f::uintX())
    , row1(Vector4f::uintY())
    , row2(Vector4f::uintZ())
    , row3(Vector4f::uintW())
{}

Matrix4f::Matrix4f(Vector4f _row0, Vector4f _row1, Vector4f _row2, Vector4f _row3)
    : row0(_row0), row1(_row1), row2(_row2), row3(_row3)
{}

Matrix4f::Matrix4f(float m00, float m01, float m02, float m03,
         float m10, float m11, float m12, float m13,
         float m20, float m21, float m22, float m23,
         float m30, float m31, float m32, float m33)
    : row0(m00, m01, m02, m03)
    , row1(m10, m11, m12, m13)
    , row2(m20, m21, m22, m23)
    , row3(m30, m31, m32, m33)
{}

std::string Matrix4f::str() {
    std::stringstream ss;
    ss << "{" << row0.str() << ", " << row1.str() << ", " << row2.str() << ", " << row3.str() << "}";
    return ss.str();
}

Vector4f Matrix4f::col0() { return Vector4f(row0.x, row1.x, row2.x, row3.x); }
Vector4f Matrix4f::col1() { return Vector4f(row0.y, row1.y, row2.y, row3.y); }
Vector4f Matrix4f::col2() { return Vector4f(row0.z, row1.z, row2.z, row3.z); }
Vector4f Matrix4f::col3() { return Vector4f(row0.w, row1.w, row2.w, row3.w); }

float& Matrix4f::operator[](int index) {
    switch(index) {
    case 0:
        return row0.x;
    case 1:
        return row0.y;
    case 2:
        return row0.z;
    case 3:
        return row0.w;
    case 4:
        return row1.x;
    case 5:
        return row1.y;
    case 6:
        return row1.z;
    case 7:
        return row1.w;
    case 8:
        return row2.x;
    case 9:
        return row2.y;
    case 10:
        return row2.z;
    case 11:
        return row2.w;
    case 12:
        return row3.x;
    case 13:
        return row3.y;
    case 14:
        return row3.z;
    case 15:
        return row3.w;
    default:
        throw new std::runtime_error("Index out of range!");
    }
}

Matrix4f Matrix4f::operator*(const Matrix4f& rhs) {
    Matrix4f m;
    multiply(*this, rhs, m);
    return m;
}

float Matrix4f::determinant() {
    float m11 = row0.x, m12 = row0.y, m13 = row0.z, m14 = row0.w,
          m21 = row1.x, m22 = row1.y, m23 = row1.z, m24 = row1.w,
          m31 = row2.x, m32 = row2.y, m33 = row2.z, m34 = row2.w,
          m41 = row3.x, m42 = row3.y, m43 = row3.z, m44 = row3.w;
    return m11 * m22 * m33 * m44 - m11 * m22 * m34 * m43 + m11 * m23 * m34 * m42 - m11 * m23 * m32 * m44
         + m11 * m24 * m32 * m43 - m11 * m24 * m33 * m42 - m12 * m23 * m34 * m41 + m12 * m23 * m31 * m44
         - m12 * m24 * m31 * m43 + m12 * m24 * m33 * m41 - m12 * m21 * m33 * m44 + m12 * m21 * m34 * m43
         + m13 * m24 * m31 * m42 - m13 * m24 * m32 * m41 + m13 * m21 * m32 * m44 - m13 * m21 * m34 * m42
         + m13 * m22 * m34 * m41 - m13 * m22 * m31 * m44 - m14 * m21 * m32 * m43 + m14 * m21 * m33 * m42
         - m14 * m22 * m33 * m41 + m14 * m22 * m31 * m43 - m14 * m23 * m31 * m42 + m14 * m23 * m32 * m41;
}

Vector4f Matrix4f::diagonal() const { return Vector4f(row0.x, row1.y, row2.z, row3.w); }
void Matrix4f::diagonal(Vector4f const& v) {
    row0.x = v.x;
    row1.y = v.y;
    row2.z = v.z;
    row3.w = v.w;
}

float Matrix4f::trace() { return row0.x + row1.y + row2.z + row3.w; }

void Matrix4f::normalize() {
    float d = determinant();
    row0 /= d;
    row1 /= d;
    row2 /= d;
    row3 /= d;
}
Matrix4f Matrix4f::normalized() {
    Matrix4f m(*this);
    m.normalize();
    return m;
}

Matrix4f Matrix4f::invert() { return invert(*this); }
Matrix4f Matrix4f::invert(Matrix4f& m) {
    if (m.determinant() == 0) return m;
    Matrix4f inv = Matrix4f::identity();
    inv[0] = m[5] * m[10] * m[15] -
             m[5] * m[11] * m[14] -
             m[9] * m[6] * m[15] +
             m[9] * m[7] * m[14] +
             m[13] * m[6] * m[11] -
             m[13] * m[7] * m[10];

    inv[4] = -m[4] * m[10] * m[15] +
             m[4] * m[11] * m[14] +
             m[8] * m[6] * m[15] -
             m[8] * m[7] * m[14] -
             m[12] * m[6] * m[11] +
             m[12] * m[7] * m[10];

    inv[8] = m[4] * m[9] * m[15] -
             m[4] * m[11] * m[13] -
             m[8] * m[5] * m[15] +
             m[8] * m[7] * m[13] +
             m[12] * m[5] * m[11] -
             m[12] * m[7] * m[9];

    inv[12] = -m[4] * m[9] * m[14] +
              m[4] * m[10] * m[13] +
              m[8] * m[5] * m[14] -
              m[8] * m[6] * m[13] -
              m[12] * m[5] * m[10] +
              m[12] * m[6] * m[9];

    inv[1] = -m[1] * m[10] * m[15] +
             m[1] * m[11] * m[14] +
             m[9] * m[2] * m[15] -
             m[9] * m[3] * m[14] -
             m[13] * m[2] * m[11] +
             m[13] * m[3] * m[10];

    inv[5] = m[0] * m[10] * m[15] -
             m[0] * m[11] * m[14] -
             m[8] * m[2] * m[15] +
             m[8] * m[3] * m[14] +
             m[12] * m[2] * m[11] -
             m[12] * m[3] * m[10];

    inv[9] = -m[0] * m[9] * m[15] +
             m[0] * m[11] * m[13] +
             m[8] * m[1] * m[15] -
             m[8] * m[3] * m[13] -
             m[12] * m[1] * m[11] +
             m[12] * m[3] * m[9];

    inv[13] = m[0] * m[9] * m[14] -
              m[0] * m[10] * m[13] -
              m[8] * m[1] * m[14] +
              m[8] * m[2] * m[13] +
              m[12] * m[1] * m[10] -
              m[12] * m[2] * m[9];

    inv[2] = m[1] * m[6] * m[15] -
             m[1] * m[7] * m[14] -
             m[5] * m[2] * m[15] +
             m[5] * m[3] * m[14] +
             m[13] * m[2] * m[7] -
             m[13] * m[3] * m[6];

    inv[6] = -m[0] * m[6] * m[15] +
             m[0] * m[7] * m[14] +
             m[4] * m[2] * m[15] -
             m[4] * m[3] * m[14] -
             m[12] * m[2] * m[7] +
             m[12] * m[3] * m[6];

    inv[10] = m[0] * m[5] * m[15] -
              m[0] * m[7] * m[13] -
              m[4] * m[1] * m[15] +
              m[4] * m[3] * m[13] +
              m[12] * m[1] * m[7] -
              m[12] * m[3] * m[5];

    inv[14] = -m[0] * m[5] * m[14] +
              m[0] * m[6] * m[13] +
              m[4] * m[1] * m[14] -
              m[4] * m[2] * m[13] -
              m[12] * m[1] * m[6] +
              m[12] * m[2] * m[5];

    inv[3] = -m[1] * m[6] * m[11] +
             m[1] * m[7] * m[10] +
             m[5] * m[2] * m[11] -
             m[5] * m[3] * m[10] -
             m[9] * m[2] * m[7] +
             m[9] * m[3] * m[6];

    inv[7] = m[0] * m[6] * m[11] -
             m[0] * m[7] * m[10] -
             m[4] * m[2] * m[11] +
             m[4] * m[3] * m[10] +
             m[8] * m[2] * m[7] -
             m[8] * m[3] * m[6];

    inv[11] = -m[0] * m[5] * m[11] +
              m[0] * m[7] * m[9] +
              m[4] * m[1] * m[11] -
              m[4] * m[3] * m[9] -
              m[8] * m[1] * m[7] +
              m[8] * m[3] * m[5];

    inv[15] = m[0] * m[5] * m[10] -
              m[0] * m[6] * m[9] -
              m[4] * m[1] * m[10] +
              m[4] * m[2] * m[9] +
              m[8] * m[1] * m[6] -
              m[8] * m[2] * m[5];

    float det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

    if (det == 0) {
        throw new std::runtime_error("Matrix is singular and cannot be inverted.");
    } else {
        det = 1.0f / det;
        for (int i = 0; i < 16; i++)
            inv[i] = inv[i] * det;
    }
    return inv;
}

void Matrix4f::multiply(const Matrix4f& lhs, const Matrix4f& rhs, Matrix4f& out) {
    float lM11 = lhs.row0.x, lM12 = lhs.row0.y, lM13 = lhs.row0.z, lM14 = lhs.row0.w,
          lM21 = lhs.row1.x, lM22 = lhs.row1.y, lM23 = lhs.row1.z, lM24 = lhs.row1.w,
          lM31 = lhs.row2.x, lM32 = lhs.row2.y, lM33 = lhs.row2.z, lM34 = lhs.row2.w,
          lM41 = lhs.row3.x, lM42 = lhs.row3.y, lM43 = lhs.row3.z, lM44 = lhs.row3.w,
          rM11 = rhs.row0.x, rM12 = rhs.row0.y, rM13 = rhs.row0.z, rM14 = rhs.row0.w,
          rM21 = rhs.row1.x, rM22 = rhs.row1.y, rM23 = rhs.row1.z, rM24 = rhs.row1.w,
          rM31 = rhs.row2.x, rM32 = rhs.row2.y, rM33 = rhs.row2.z, rM34 = rhs.row2.w,
          rM41 = rhs.row3.x, rM42 = rhs.row3.y, rM43 = rhs.row3.z, rM44 = rhs.row3.w;

    out.row0.x = (((lM11 * rM11) + (lM12 * rM21)) + (lM13 * rM31)) + (lM14 * rM41);
    out.row0.y = (((lM11 * rM12) + (lM12 * rM22)) + (lM13 * rM32)) + (lM14 * rM42);
    out.row0.z = (((lM11 * rM13) + (lM12 * rM23)) + (lM13 * rM33)) + (lM14 * rM43);
    out.row0.w = (((lM11 * rM14) + (lM12 * rM24)) + (lM13 * rM34)) + (lM14 * rM44);
    out.row1.x = (((lM21 * rM11) + (lM22 * rM21)) + (lM23 * rM31)) + (lM24 * rM41);
    out.row1.y = (((lM21 * rM12) + (lM22 * rM22)) + (lM23 * rM32)) + (lM24 * rM42);
    out.row1.z = (((lM21 * rM13) + (lM22 * rM23)) + (lM23 * rM33)) + (lM24 * rM43);
    out.row1.w = (((lM21 * rM14) + (lM22 * rM24)) + (lM23 * rM34)) + (lM24 * rM44);
    out.row2.x = (((lM31 * rM11) + (lM32 * rM21)) + (lM33 * rM31)) + (lM34 * rM41);
    out.row2.y = (((lM31 * rM12) + (lM32 * rM22)) + (lM33 * rM32)) + (lM34 * rM42);
    out.row2.z = (((lM31 * rM13) + (lM32 * rM23)) + (lM33 * rM33)) + (lM34 * rM43);
    out.row2.w = (((lM31 * rM14) + (lM32 * rM24)) + (lM33 * rM34)) + (lM34 * rM44);
    out.row3.x = (((lM41 * rM11) + (lM42 * rM21)) + (lM43 * rM31)) + (lM44 * rM41);
    out.row3.y = (((lM41 * rM12) + (lM42 * rM22)) + (lM43 * rM32)) + (lM44 * rM42);
    out.row3.z = (((lM41 * rM13) + (lM42 * rM23)) + (lM43 * rM33)) + (lM44 * rM43);
    out.row3.w = (((lM41 * rM14) + (lM42 * rM24)) + (lM43 * rM34)) + (lM44 * rM44);
}

Matrix4f Matrix4f::createTranslation(const Vector3f& v) { return createTranslation(v.x, v.y, v.z); }
Matrix4f Matrix4f::createTranslation(float x, float y, float z) {
    Matrix4f m = identity();
    m.row3.x = x;
    m.row3.y = y;
    m.row3.z = z;
    return m;
}

Matrix4f Matrix4f::createScale(const Vector3f& v) { return createScale(v.x, v.y, v.z); }
Matrix4f Matrix4f::createScale(float value) { return createScale(value, value, value); }
Matrix4f Matrix4f::createScale(float x, float y, float z) {
    Matrix4f m = identity();
    m.row0.x = x;
    m.row1.y = y;
    m.row2.z = z;
    return m;
}

Matrix4f Matrix4f::createFromAxisAngle(Vector3f axis, float angle) {
    Matrix4f m;

    axis.normalize();
    float axisX = axis.x, axisY = axis.y, axisZ = axis.z;

    float cos = std::cos(-angle);
    float sin = std::sin(-angle);
    float t = 1.0f - cos;

    float tXX = t * axisX * axisX,
          tXY = t * axisX * axisY,
          tXZ = t * axisX * axisZ,
          tYY = t * axisY * axisY,
          tYZ = t * axisY * axisZ,
          tZZ = t * axisZ * axisZ;

    float sinX = sin * axisX,
          sinY = sin * axisY,
          sinZ = sin * axisZ;

    m.row0.x = tXX + cos;
    m.row0.y = tXY - sinZ;
    m.row0.z = tXZ + sinY;
    m.row0.w = 0;
    m.row1.x = tXY + sinZ;
    m.row1.y = tYY + cos;
    m.row1.z = tYZ - sinX;
    m.row1.w = 0;
    m.row2.x = tXZ - sinY;
    m.row2.y = tYZ + sinX;
    m.row2.z = tZZ + cos;
    m.row2.w = 0;
    m.row3 = Vector4f::uintW();
    return m;
}

Matrix4f Matrix4f::identity() { return Matrix4f(Vector4f::uintX(), Vector4f::uintY(), Vector4f::uintZ(), Vector4f::uintW()); }
Matrix4f Matrix4f::zero() { return Matrix4f(Vector4f::zero(),Vector4f::zero(), Vector4f::zero(), Vector4f::zero()); }

}
