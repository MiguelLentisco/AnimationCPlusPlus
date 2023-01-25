#include "Core/Vec3.h"

#include <stdexcept>

#include "Core/BasicUtils.h"

// ---------------------------------------------------------------------------------------------------------------------

float& Vec3::operator[](int i)
{
    if (i < 0 || i > 2)
    {
        throw std::invalid_argument("[i] must be 0, 1 or 2");
    }
    
    return i == 0 ? x : (i == 1 ? y : z);
    
} // operator[]

// ---------------------------------------------------------------------------------------------------------------------

const float& Vec3::operator[](int i) const
{
    if (i < 0 || i > 2)
    {
        throw std::invalid_argument("[i] must be 0, 1 or 2");
    }
    
    return i == 0 ? x : (i == 1 ? y : z);
    
} // operator[]

// ---------------------------------------------------------------------------------------------------------------------

bool Vec3::operator==(const Vec3& v) const
{
    const Vec3 diff = *this - v;
    return diff.IsZeroVec();
    
} // operator==

// ---------------------------------------------------------------------------------------------------------------------

bool Vec3::operator!=(const Vec3& v) const
{
    return !(*this == v);
    
} // operator!=

// ---------------------------------------------------------------------------------------------------------------------

Vec3 Vec3::operator-() const
{
    return {-x, -y, -z};
    
} // operator-

// ---------------------------------------------------------------------------------------------------------------------

Vec3 Vec3::operator+(const Vec3& v) const
{
    return {x + v.x, y + v.y, z + v.z};
    
} // operator+

// ---------------------------------------------------------------------------------------------------------------------

Vec3 Vec3::operator-(const Vec3& v) const
{
    return {x - v.x, y - v.y, z - v.z};
    
} // operator-

// ---------------------------------------------------------------------------------------------------------------------

Vec3 Vec3::operator*(float f) const
{
    return {x * f, y * f, z * f};
    
} // operator*

// ---------------------------------------------------------------------------------------------------------------------

Vec3 Vec3::operator*(const Vec3& v) const
{
    return {x * v.x, y * v.y, z * v.z};
    
} // operator*

// ---------------------------------------------------------------------------------------------------------------------

Vec3 Vec3::operator/(float f) const
{
    const float scale = 1.f / f;
    return {x * scale, y * scale, z * scale};
    
} // operator/

// ---------------------------------------------------------------------------------------------------------------------

Vec3 Vec3::operator/(const Vec3& v) const
{
    return {x / v.x, y / v.y, z / v.z };
    
} // operator/

// ---------------------------------------------------------------------------------------------------------------------

Vec3& Vec3::operator+=(float f)
{
    x += f;
    y += f;
    z += f;
    
    return *this;
    
} // operator+=

// ---------------------------------------------------------------------------------------------------------------------

Vec3& Vec3::operator+=(const Vec3& v)
{
    x += v.x;
    y += v.y;
    z += v.z;

    return *this;
    
} // operator+=

// ---------------------------------------------------------------------------------------------------------------------

Vec3& Vec3::operator-=(float f)
{
    x -= f;
    y -= f;
    z -= f;
    
    return *this;
    
} // operator-=

// ---------------------------------------------------------------------------------------------------------------------

Vec3& Vec3::operator-=(const Vec3& v)
{
    x -= v.x;
    y -= v.y;
    z -= v.z;

    return *this;
    
} // operator-=

// ---------------------------------------------------------------------------------------------------------------------

Vec3& Vec3::operator*=(float f)
{
    x *= f;
    y *= f;
    z *= f;
    
    return *this;
    
} // operator*=

// ---------------------------------------------------------------------------------------------------------------------

Vec3& Vec3::operator*=(const Vec3& v)
{
    x *= v.x;
    y *= v.y;
    z *= v.z;

    return *this;
    
} // operator*=

// ---------------------------------------------------------------------------------------------------------------------

bool Vec3::IsZeroVec() const
{
    return BasicUtils::IsZero(LenSq());
    
} // IsZeroVec

// ---------------------------------------------------------------------------------------------------------------------

float Vec3::Dot(const Vec3& u, const Vec3& v)
{
    return u.x * v.x + u.y * v.y + u.z * v.z;
    
} // Dot

// ---------------------------------------------------------------------------------------------------------------------

float Vec3::Dot(const Vec3& v) const
{
    return Dot(*this, v);
    
} // dot

// ---------------------------------------------------------------------------------------------------------------------

float Vec3::operator|(const Vec3& v) const
{
    return Dot(v);
    
} // operator|

// ---------------------------------------------------------------------------------------------------------------------

Vec3 Vec3::Cross(const Vec3& u, const Vec3& v)
{
    return { u.y * v.z - u.z * v.y, u.z * v.x - u.x * v.z, u.x * v.y - u.y * v.x }; 
    
} // Cross

// ---------------------------------------------------------------------------------------------------------------------

Vec3 Vec3::Cross(const Vec3& v) const
{
    return Cross(*this, v);
    
} // Cross

// ---------------------------------------------------------------------------------------------------------------------

Vec3 Vec3::operator^(const Vec3& v) const
{
    return Cross(*this, v);
    
} // operator^

// ---------------------------------------------------------------------------------------------------------------------

float Vec3::Len() const
{
    const float lenSq = LenSq();
    return BasicUtils::IsZero(lenSq) ? .0f : sqrtf(lenSq);
    
} // Len

// ---------------------------------------------------------------------------------------------------------------------

float Vec3::LenSq() const
{
    return Dot(*this);
    
} // LenSq

// ---------------------------------------------------------------------------------------------------------------------

float Vec3::Dist(const Vec3& u, const Vec3& v)
{
    return (v - u).Len();
    
} // Dist

// ---------------------------------------------------------------------------------------------------------------------

float Vec3::Dist(const Vec3& v) const
{
    return Dist(*this, v);
    
} // Dist

// ---------------------------------------------------------------------------------------------------------------------

float Vec3::DistSq(const Vec3& u, const Vec3& v)
{
    return (v - u).LenSq();
    
} // DistSq

// ---------------------------------------------------------------------------------------------------------------------

float Vec3::DistSq(const Vec3& v) const
{
    return DistSq(*this, v);
    
} // DistSq

// ---------------------------------------------------------------------------------------------------------------------

void Vec3::Normalize()
{
    const float lenSq = LenSq();
    if (BasicUtils::IsZero(lenSq) || BasicUtils::AreEqual(lenSq, 1.f))
    {
        return;
    }

    const float invLen = 1.f / sqrtf(lenSq);
    x *= invLen;
    y *= invLen;
    z *= invLen;
    
} // Normalize

// ---------------------------------------------------------------------------------------------------------------------

Vec3 Vec3::Normalized() const
{
    const float lenSq = LenSq();
    if (BasicUtils::IsZero(lenSq) || BasicUtils::AreEqual(lenSq, 1.f))
    {
        return *this;
    }

    const float invLen = 1.f / sqrtf(lenSq);
    return {x * invLen, y * invLen, z * invLen};
    
} // Normalized

// ---------------------------------------------------------------------------------------------------------------------

float Vec3::Angle(const Vec3& u, const Vec3& v)
{
    const float uLenSq = u.LenSq();
    const float vLenSq = v.LenSq();
    if (BasicUtils::IsZero(uLenSq) || BasicUtils::IsZero(vLenSq))
    {
        return 0.f;
    }

    const float dot = u | v;
    const float lenProd = sqrtf(uLenSq) * sqrtf(vLenSq);
    return acosf(dot / lenProd);
    
} // Angle

// ---------------------------------------------------------------------------------------------------------------------

float Vec3::Angle(const Vec3& v) const
{
    return Angle(*this, v);
    
} // Angle

// ---------------------------------------------------------------------------------------------------------------------

Vec3 Vec3::Project(const Vec3& u, const Vec3& v)
{
    const float modV = v.Len();
    if (BasicUtils::IsZero(modV))
    {
        return {};
    }

    const float scale = (u | v) / modV;
    return v * scale;
    
} // Project

// ---------------------------------------------------------------------------------------------------------------------

Vec3 Vec3::Project(const Vec3& v) const
{
    return Project(*this, v);

} // Project

// ---------------------------------------------------------------------------------------------------------------------

Vec3 Vec3::Reject(const Vec3& u, const Vec3& v)
{
    return u - u.Project(v);
    
} // Reject

// ---------------------------------------------------------------------------------------------------------------------

Vec3 Vec3::Reject(const Vec3& v) const
{
    return Reject(*this, v);
    
} // Reject

// ---------------------------------------------------------------------------------------------------------------------

Vec3 Vec3::Reflect(const Vec3& u, const Vec3& v)
{
    const float modV = v.Len();
    if (BasicUtils::IsZero(modV))
    {
        return {};
    }

    const float scale = (u | v) / modV;
    const Vec3 doubleProj = v * (2.f * scale);
    return u - doubleProj;
    
} // Reflect

// ---------------------------------------------------------------------------------------------------------------------

Vec3 Vec3::Reflect(const Vec3& v) const
{
    return Reflect(*this, v);
    
} // Reflect

// ---------------------------------------------------------------------------------------------------------------------

Vec3 Vec3::Lerp(const Vec3& u, const Vec3& v, float t)
{
    return u + (v - u) * t;
    
} // Lerp

// ---------------------------------------------------------------------------------------------------------------------

Vec3 Vec3::SLerp(const Vec3& u, const Vec3& v, float t)
{
    const Vec3 uNorm = u.Normalized();
    const Vec3 vNorm = v.Normalized();
    const float angleUV = Angle(u,v);

    static constexpr float MIN_ANGLE = .01f;
    if (angleUV < MIN_ANGLE)
    {
        return Lerp(uNorm, vNorm, t);
    }

    const float a = sinf((1.f - t) * angleUV);
    const float b = sinf(t * angleUV);
    return (uNorm * a + vNorm * b) / sinf(angleUV);
    
} // SLerp

// ---------------------------------------------------------------------------------------------------------------------

Vec3 Vec3::NLerp(const Vec3& u, const Vec3& v, float t)
{
    return Lerp(u, v, t).Normalized();
    
} // NLerp

// ---------------------------------------------------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& os, const Vec3& v)
{
    os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
    return os;
    
} // operator<<

// ---------------------------------------------------------------------------------------------------------------------
