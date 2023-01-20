#include "Core/Mat4.h"

#include "Core/Vec3.h"
#include "Utils/Utils.h"
#include <iostream>
#include <utility>
#include <stdexcept>

// ---------------------------------------------------------------------------------------------------------------------

const float& Mat4::operator[](int i) const
{
    if (i < 0 || i > 15)
    {
        throw std::invalid_argument("[i] must be in [0,15]");
    }

    return v[i];
    
} // operator

// ---------------------------------------------------------------------------------------------------------------------

float& Mat4::operator[](int i)
{
    if (i < 0 || i > 15)
    {
        throw std::invalid_argument("[i] must be in [0,15]");
    }

    return v[i];
    
} // operator[]

// ---------------------------------------------------------------------------------------------------------------------

const float& Mat4::operator()(int i, int j) const
{
    if (i < 0 || i > 3 || j < 0 || j > 3)
    {
        throw std::invalid_argument("[i, j] must be [0,3]x[0,3]");
    }

    return v[i * 4 + j];
    
} // operator()

// ---------------------------------------------------------------------------------------------------------------------

float& Mat4::operator()(int i, int j)
{
    if (i < 0 || i > 3 || j < 0 || j > 3)
    {
        throw std::invalid_argument("[i, j] must be [0,3]x[0,3]");
    }

    return v[i * 4 + j];
    
} // operator[]

// ---------------------------------------------------------------------------------------------------------------------

bool Mat4::operator==(const Mat4& m) const
{
    for (int i = 0; i < 16; ++i)
    {
        if (!Utils::AreEqual(v[i], m.v[i]))
        {
            return false;
        }
    }
    return true;
    
} // operator==

// ---------------------------------------------------------------------------------------------------------------------

bool Mat4::operator!=(const Mat4& m) const
{
    return !(*this == m);
    
} // operator!=

// ---------------------------------------------------------------------------------------------------------------------

Mat4 Mat4::operator+(const Mat4& m) const
{
   return
    {
       xx + m.xx, xy + m.xy, xz + m.xz, xw + m.xw,
       yx + m.yx, yy + m.yy, yz + m.yz, yw + m.yw,
       zx + m.zx, zy + m.zy, zz + m.zz, zw + m.zw,
       tx + m.tx, ty + m.ty, tz + m.tz, tw + m.tw
   };
    
} // operator+

// ---------------------------------------------------------------------------------------------------------------------

Mat4 Mat4::operator*(float f) const
{
    return
    {
        xx * f, xy * f, xz * f, xw * f,
        yx * f, yy * f, yz * f, yw * f,
        zx * f, zy * f, zz * f, zw * f,
        tx * f, ty * f, tz * f, tw * f
    };
    
} // operator*

// ---------------------------------------------------------------------------------------------------------------------

Mat4 Mat4::operator*(const Mat4& m) const
{
    return
    {
        M4D(0, 0), M4D(1, 0), M4D(2, 0), M4D(3, 0),
        M4D(0, 1), M4D(1, 1), M4D(2, 1), M4D(3, 1),
        M4D(0, 2), M4D(1, 2), M4D(2, 2), M4D(3, 2),
        M4D(0, 3), M4D(1, 3), M4D(2, 3), M4D(3, 3),
    };
    
} // operator*

// ---------------------------------------------------------------------------------------------------------------------

Mat4 Mat4::operator*(const Vec4& v) const
{
    return
    {
        M4V4D(0),
        M4V4D(1),
        M4V4D(2),
        M4V4D(3),
    };
    
} // operator*

// ---------------------------------------------------------------------------------------------------------------------

Vec3 Mat4::TransformVector(const Vec3& v) const
{
    return
    {
        M4V3D(0),
        M4V3D(1),
        M4V3D(2)
    };
    
} // TransformVector

// ---------------------------------------------------------------------------------------------------------------------

Vec3 Mat4::TransformPoint(const Vec3& v) const
{
    return
    {
        M4P3D(0),
        M4P3D(1),
        M4P3D(2)
    };
    
} // TransformPoint

// ---------------------------------------------------------------------------------------------------------------------

Vec3 Mat4::TransformPoint(const Vec3& v, float& w) const
{
    const float newW = w;
    w = M4V3WD(3, w);

    return
    {
        M4V3WD(0, newW),
        M4V3WD(1, newW),
        M4V3WD(2, newW)
    };
    
} // TransformPoint

// ---------------------------------------------------------------------------------------------------------------------

void Mat4::Transpose()
{
    std::swap(yx, xy);
    std::swap(zx, xz);
    std::swap(tx, xw);
    std::swap(zy, yz);
    std::swap(ty, yw);
    std::swap(tz, zw);
    
} // Transpose

// ---------------------------------------------------------------------------------------------------------------------

Mat4 Mat4::Transposed() const
{
    return {
        xx, yx, zx, tx,
        xy, yy, zy, ty,
        xz, yz, zz, tz,
        xw, yw, zw, tw
    };
    
} // Transposed

// ---------------------------------------------------------------------------------------------------------------------

float Mat4::Determinant() const
{
    return    v[0]  * M4_3X3MINOR(1, 2, 3, 1, 2, 3)
            - v[4]  * M4_3X3MINOR(0, 2, 3, 1, 2, 3)
            + v[8]  * M4_3X3MINOR(0, 1, 3, 1, 2, 3)
            - v[12] * M4_3X3MINOR(0, 1, 2, 1, 2, 3);
    
} // Determinant

// ---------------------------------------------------------------------------------------------------------------------

Mat4 Mat4::Adjugate() const
{
    // Adjugate(i, j) = Cofactor(M[j, i]) = Minor(M[j, i]) * -1^{i+j}
    return
    {
        M4_3X3MINOR(1, 2, 3, 1, 2, 3), -M4_3X3MINOR(0, 2, 3, 1, 2, 3), M4_3X3MINOR(0, 1, 3, 1, 2, 3), -M4_3X3MINOR(0, 1, 2, 1, 2, 3),
        -M4_3X3MINOR(1, 2, 3, 0, 2, 3), M4_3X3MINOR(0, 2, 3, 0, 2, 3), -M4_3X3MINOR(0, 1, 3, 0, 2, 3), M4_3X3MINOR(0, 1, 2, 0, 2, 3),
        M4_3X3MINOR(1, 2, 3, 0, 1, 3), -M4_3X3MINOR(0, 2, 3, 0, 1, 3), M4_3X3MINOR(0, 1, 3, 0, 1, 3), -M4_3X3MINOR(0, 1, 2, 0, 1, 3),
        -M4_3X3MINOR(1, 2, 3, 0, 1, 2), M4_3X3MINOR(0, 2, 3, 0, 1, 2), -M4_3X3MINOR(0, 1, 3, 0, 1, 2), M4_3X3MINOR(0, 1, 2, 0, 1, 2)
    };
    
} // Adjugate

// ---------------------------------------------------------------------------------------------------------------------

void Mat4::Invert()
{
    const float det = Determinant();
    if (Utils::IsZero(det))
    {
        std::cout << "Matrix determinant is 0\n";
        return;
    }

    *this = Adjugate() * (1.f / det);
    
} // Invert

// ---------------------------------------------------------------------------------------------------------------------

Mat4 Mat4::Inverse() const
{
    const float det = Determinant();
    if (Utils::IsZero(det))
    {
        std::cout << "Matrix determinant is 0\n";
        return *this;
    }

    return Adjugate() * (1.f / det);
    
} // Inverse

// ---------------------------------------------------------------------------------------------------------------------

Mat4 Mat4::CreateFrustum(float l, float r, float b, float t, float n, float f)
{
    if (Utils::AreEqual(l, r) || Utils::AreEqual(t,  b) || Utils::AreEqual(n, f))
    {
        std::cout << "Invalid frustum\n";
        return {};
    }

    return
    {
        (2.f*n)/(r-l), 0.f, 0.f, 0.f,
        0.f, (2.f*n)/(t-b), 0.f, 0.f,
        (r+l)/(r-l), (t+b)/(t-b), -(f+n)/(f-n), -1.f,
        0.f, 0.f, (-2.f*f*n)/(f-n), 0.f 
    };
    
} // CreateFrustum

// ---------------------------------------------------------------------------------------------------------------------

Mat4 Mat4::CreatePerspective(float fov, float aspect, float n, float f)
{
    const float top = n * std::tan(Utils::DegToRad(.5f * fov));
    const float right = top * aspect;
    
    return CreateFrustum(-right, right, -top, top, n, f);
    
} // CreatePerspective

// ---------------------------------------------------------------------------------------------------------------------

Mat4 Mat4::CreateOrtho(float l, float r, float b, float t, float n, float f)
{
    if (Utils::AreEqual(l, r) || Utils::AreEqual(t,  b) || Utils::AreEqual(n, f))
    {
        std::cout << "Invalid frustum\n";
        return {};
    }

    return
    {
        2.f/(r-l), 0.f, 0.f, 0.f,
        0.f, 2.f/(t-b), 0.f, 0.f,
        0.f, 0.f, -2.f/(f-n), 0.f,
        -(r+l)/(r-l), -(t+b)/(t-b), -(f+n)/(f-n), 1.f
    };
    
} // CreateOrtho

// ---------------------------------------------------------------------------------------------------------------------

Mat4 Mat4::CreateLookAt(const Vec3& position, const Vec3& target, const Vec3& up)
{
    const Vec3 camForward = (position - target).Normalized();
    Vec3 camRight = up ^ camForward; // Right handed
    
    if (camRight.IsZeroVec())
    {
        std::cout << "Invalid frustum\n";
        return {};
    }

    camRight.Normalize();
    const Vec3 camUp = (camForward ^ camRight).Normalized(); // Right handed

    return
    {
        camRight.x, camUp.x, camForward.x, 0.f,
        camRight.y, camUp.y, camForward.y, 0.f,
        camRight.z, camUp.z, camForward.z, 0.f,
        -(camRight | position), -(camUp | position), -(camForward | position), 1.f
    };
    
} // CreateLookAt

// ---------------------------------------------------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& os, const Mat4& m)
{
    os << "\n" << m[0] << " " << m[1] << " " << m[2] << " " << m[3] << "\n" <<
        m[4] << " " << m[5] << " " << m[6] << " " << m[7] << "\n" <<
        m[8] << " " << m[9] << " " << m[10] << " " << m[11] << "\n" <<
        m[12] << " " << m[13] << " " << m[14] << " " << m[15];
    return os;
    
} // operator<<

// ---------------------------------------------------------------------------------------------------------------------

