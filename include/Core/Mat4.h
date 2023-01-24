#pragma once

#include <ostream>

#include "Core/TVec4.h"

struct Vec3;

struct Mat4
{
    union
    {
        float v[16];
        
        struct
        {
            Vec4 right;
            Vec4 up;
            Vec4 forward;
            Vec4 position;
        };

        struct
        {
            float xx; float xy; float xz; float xw;
            float yx; float yy; float yz; float yw;
            float zx; float zy; float zz; float zw;
            float tx; float ty; float tz; float tw;
        };

        struct
        {
            float c0r0; float c0r1; float c0r2; float c0r3;
            float c1r0; float c1r1; float c1r2; float c1r3;
            float c2r0; float c2r1; float c2r2; float c2r3;
            float c3r0; float c3r1; float c3r2; float c3r3;
        };

        struct
        {
            float r0c0; float r1c0; float r2c0; float r3c0;
            float r0c1; float r1c1; float r2c1; float r3c1;
            float r0c2; float r1c2; float r2c2; float r3c2;
            float r0c3; float r1c3; float r2c3; float r3c3;
        };
    };

    Mat4(float xx = 1, float xy = 0, float xz = 0, float xw = 0,
        float yx = 0, float yy = 1, float yz = 0, float yw = 0,
        float zx = 0, float zy = 0, float zz = 1, float zw = 0,
        float tx = 0, float ty = 0, float tz = 0, float tw = 1) :
        xx(xx), xy(xy), xz(xz), xw(xw),
        yx(yx), yy(yy), yz(yz), yw(yw),
        zx(zx), zy(zy), zz(zz), zw(zw),
        tx(tx), ty(ty), tz(tz), tw(tw) {}
    
    Mat4(const float v[16]) :
        xx(v[0]),  xy(v[1]),  xz(v[2]),  xw(v[3]),
        yx(v[4]),  yy(v[5]),  yz(v[6]),  yw(v[7]),
        zx(v[8]),  zy(v[9]),  zz(v[10]), zw(v[11]),
        tx(v[12]), ty(v[13]), tz(v[14]), tw(v[15]) {}

    Mat4(const Mat4& m) : Mat4(m.v) {}

    const float& operator[](int i) const;
    float& operator[](int i);
    const float& operator()(int i, int j) const;
    float& operator()(int i, int j);

    bool operator==(const Mat4& m) const;
    bool operator!=(const Mat4& m) const;

    Mat4 operator+(const Mat4& m) const;
    Mat4 operator*(float f) const;
    Mat4 operator*(const Mat4& m) const;
    Mat4 operator*(const Vec4& v) const;

    Mat4& operator+=(const Mat4& m);
    Mat4& operator*=(const Mat4& m);

    friend std::ostream& operator<<(std::ostream& os, const Mat4& m);

    Vec3 TransformVector(const Vec3& v) const;
    Vec3 TransformPoint(const Vec3& v) const;
    Vec3 TransformPoint(const Vec3& v, float& w) const;

    void Transpose();
    Mat4 Transposed() const;

    float Determinant() const;
    Mat4 Adjugate() const;
    
    void Invert();
    Mat4 Inverse() const;

    static Mat4 CreateFrustum(float l, float r, float b, float t, float n, float f);
    static Mat4 CreatePerspective(float fov, float aspect, float n, float f);
    static Mat4 CreateOrtho(float l, float r, float b, float t, float n, float f);
    static Mat4 CreateLookAt(const Vec3& position, const Vec3& target, const Vec3& up);
    
}; // Mat4

// M1[row, :] * M2[:, col]
#define M4D(row, col) \
    (this->v[0 * 4 + (row)] * m.v[(col) * 4 + 0] + \
    this->v[1 * 4 + (row)] * m.v[(col) * 4 + 1] + \
    this->v[2 * 4 + (row)] * m.v[(col) * 4 + 2] + \
    this->v[3 * 4 + (row)] * m.v[(col) * 4 + 3])

// M1[row, :] * (x, y, x, w)
#define M4P4D(row, x, y, z, w) \
    ((x) * this->v[0 * 4 + (row)] + \
    (y) * this->v[1 * 4 + (row)] + \
    (z) * this->v[2 * 4 + (row)] + \
    (w) * this->v[3 * 4 + (row)])

// M1[row, :] * 4V
#define M4V4D(row) M4P4D(row, v.x, v.y, v.z, v.w)
// M1[row, :] * (3V, w)
#define M4V3WD(row, w) M4P4D(row, v.x, v.y, v.z, w)
// M1[row, :] * 3D vector
#define M4V3D(row) M4V3WD(row, .0f)
// M1[row, :] * 3D point
#define M4P3D(row) M4V3WD(row, 1.f)

// Calc M_{4x4} minor as det(M_{3x3}), M_{3X3} is given by M_{c_ir_i}
#define M4_3X3MINOR(c0, c1, c2, r0, r1, r2) \
    (v[(c0)*4+(r0)]*(v[(c1)*4+(r1)]*v[(c2)*4+(r2)]-v[(c1)*4+(r2)]* \
    v[(c2)*4+(r1)])-v[(c1)*4+(r0)]*(v[(c0)*4+(r1)]*v[(c2)*4+(r2)]- \
    v[(c0)*4+(r2)]*v[(c2)*4+(r1)])+v[(c2)*4+(r0)]*(v[(c0)*4+(r1)]* \
    v[(c1)*4+(r2)]-v[(c0)*4+(r2)]*v[(c1)*4+(r1)]))

