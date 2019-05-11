#pragma once

#include "shendk/types/vector.h"

namespace shendk {

struct Matrix4f {
    Vector4f row0;
    Vector4f row1;
    Vector4f row2;
    Vector4f row3;

    Matrix4f();

    Matrix4f(Vector4f _row0, Vector4f _row1, Vector4f _row2, Vector4f _row3);

    Matrix4f(float m00, float m01, float m02, float m03,
             float m10, float m11, float m12, float m13,
             float m20, float m21, float m22, float m23,
             float m30, float m31, float m32, float m33);

    Vector4f col0();
    Vector4f col1();
    Vector4f col2();
    Vector4f col3();

    std::string str();

    float& operator[](int index);

    Matrix4f operator*(const Matrix4f& rhs);

    float determinant();

    Vector4f diagonal() const;
    void diagonal(Vector4f const& v);

    float trace();

    void normalize();
    Matrix4f normalized();

    Matrix4f invert();
    static Matrix4f invert(Matrix4f& m);

    static void multiply(const Matrix4f& lhs, const Matrix4f& rhs, Matrix4f& out);

    static Matrix4f createTranslation(const Vector3f& v);
    static Matrix4f createTranslation(float x, float y, float z);

    static Matrix4f createScale(const Vector3f& v);
    static Matrix4f createScale(float value);
    static Matrix4f createScale(float x, float y, float z);

    static Matrix4f createFromAxisAngle(Vector3f axis, float angle);

    static Matrix4f identity();
    static Matrix4f zero();

};

}
