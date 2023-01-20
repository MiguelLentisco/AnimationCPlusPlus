#include "Core/Quat.h"

#include "Core/Mat4.h"
#include "Utils/Utils.h"

// ---------------------------------------------------------------------------------------------------------------------

Vec3 Quat::GetAxis() const
{
    return vector.Normalized();
    
} // GetAxis

// ---------------------------------------------------------------------------------------------------------------------

float Quat::GetAngle() const
{
    return 2.f * std::acos(scalar);
    
} // GetAngle

// ---------------------------------------------------------------------------------------------------------------------

float Quat::LenSq() const
{
    return *this | *this;
    
} // LenSq

// ---------------------------------------------------------------------------------------------------------------------

float Quat::Len() const
{
    const float lenSq = LenSq();
    return Utils::IsZero(lenSq) ? 0.f : std::sqrt(lenSq);
    
} // Len

// ---------------------------------------------------------------------------------------------------------------------

void Quat::Normalize()
{
    const float lenSq = LenSq();
    if (Utils::IsZero(lenSq))
    {
        return;
    }

    const float divLen = 1.f / std::sqrt(lenSq);
    vector *= divLen;
    scalar *= scalar;
    
} // Normalize

// ---------------------------------------------------------------------------------------------------------------------

Quat Quat::Normalized() const
{
    const float lenSq = LenSq();
    if (Utils::IsZero(lenSq))
    {
        return {};
    }

    const float divLen = 1.f / std::sqrt(lenSq);
    return *this * divLen;
    
} // Normalized

// ---------------------------------------------------------------------------------------------------------------------

Quat Quat::Conjugate() const
{
    return {-vector, scalar};
    
} // Conjugate

// ---------------------------------------------------------------------------------------------------------------------

Quat Quat::Inverse() const
{
    const float lenSq = LenSq();
    if (Utils::IsZero(lenSq))
    {
        return {};    
    }

    const float lenSqDiv = 1.f / lenSq;
    return {vector * -lenSqDiv, scalar * lenSqDiv};
    
} // Inverse

// ---------------------------------------------------------------------------------------------------------------------

Quat Quat::operator-() const
{
    return {-vector, -scalar};
    
} // operator

// ---------------------------------------------------------------------------------------------------------------------

Quat Quat::operator+(const Quat& q) const
{
    return {vector + q.vector, scalar + q.scalar};
    
} // operator+

// ---------------------------------------------------------------------------------------------------------------------

Quat Quat::operator-(const Quat& q) const
{
    return {vector - q.vector, scalar - q.scalar};
    
} // operator-

// ---------------------------------------------------------------------------------------------------------------------

Quat Quat::operator*(float f) const
{
    return {vector * f, scalar * f};
    
} // operator*

// ---------------------------------------------------------------------------------------------------------------------

Quat Quat::operator*(const Quat& q) const
{
    return
    {
         q.x * w + q.y * z - q.z * y + q.w * x,
        -q.x * z + q.y * w + q.z * x + q.w * y,
         q.x * y - q.y * x + q.z * w + q.w * z,
        -q.x * x - q.y * y - q.z * z + q.w * w
    };
    
} // operator*

// ---------------------------------------------------------------------------------------------------------------------

Vec3 Quat::operator*(const Vec3& v) const
{
    return vector * 2.f * (vector | v) + v * (scalar * scalar - vector.LenSq()) + (vector ^ v) * 2.f * scalar;
    
} // operator*

// ---------------------------------------------------------------------------------------------------------------------

Quat& Quat::operator*=(float f)
{
    vector *= f;
    scalar *= f;
    
    return *this;
    
} // operator*=

// ---------------------------------------------------------------------------------------------------------------------

float Quat::operator|(const Quat& q) const
{
    return (vector | q.vector) + scalar * q.scalar;
    
} // operator|

// ---------------------------------------------------------------------------------------------------------------------

float Quat::Dot(const Quat& q1, const Quat& q2)
{
    return q1 | q2;
    
} // Dot

// ---------------------------------------------------------------------------------------------------------------------

float Quat::Dot(const Quat& q) const
{
    return Dot(*this, q);
    
} // Dot

// ---------------------------------------------------------------------------------------------------------------------

Quat Quat::operator^(float f) const
{
    const float angle = 2.f * std::cos(scalar);
    const Vec3 axis = vector.Normalized();

    const float evalAngle = f * angle * .5f;
    const float halfCos = std::cos(evalAngle);
    const float halfSin = std::sin(evalAngle);

    return {axis * halfSin, halfCos};
    
} // operator^

// ---------------------------------------------------------------------------------------------------------------------

bool Quat::operator==(const Quat& q) const
{
    return vector == q.vector && Utils::AreEqual(scalar, q.scalar);
    
} // operator==

// ---------------------------------------------------------------------------------------------------------------------

bool Quat::operator!=(const Quat& q) const
{
    return !(*this == q);
    
} // operator!=

// ---------------------------------------------------------------------------------------------------------------------

bool Quat::WithSameOrientation(const Quat& q1, const Quat& q2)
{
    return q1 == q2 || q1 == -q2;
    
} // WithSameOrientation

// ---------------------------------------------------------------------------------------------------------------------

bool Quat::WithSameOrientation(const Quat& q) const
{
    return WithSameOrientation(*this, q);
    
} // WithSameOrientation

// ---------------------------------------------------------------------------------------------------------------------

Quat Quat::GetNeighbour(const Quat& q, const Quat& other)
{
    return (q | other) >= 0.f ? q : -q;
    
} // GetNeighbour

// ---------------------------------------------------------------------------------------------------------------------

Quat Quat::GetNeighbour(const Quat& other) const
{
    return GetNeighbour(*this, other);
    
} // GetNeighbour

// ---------------------------------------------------------------------------------------------------------------------

Quat Quat::CreateFromAxis(float angle, const Vec3& axis)
{
    const float halfAngle = angle * .5f;
    const Vec3 axisNorm = axis.Normalized();

    return{axisNorm * std::sin(halfAngle), std::cos(halfAngle)};
    
} // CreateFromAxis

// ---------------------------------------------------------------------------------------------------------------------

Quat Quat::FromTo(const Vec3& from, const Vec3& to)
{
    const Vec3 fromUnit = from.Normalized();
    const Vec3 toUnit = to.Normalized();

    if (fromUnit == toUnit)
    {
        return {};
    }
    
    if (fromUnit == -toUnit)
    {
        Vec3 ortho = {1.f, 0.f, 0.f};
        if (std::abs(fromUnit.y) < std::abs(fromUnit.x))
        {
            ortho = Vec3(0.f, 1.f, 0.f);
        }
        else if (std::abs(fromUnit.z) < std::abs(fromUnit.y) && std::abs(fromUnit.z) < std::abs(fromUnit.x))
        {
            ortho = Vec3(0.f, 0.f, 1.f);    
        }

        const Vec3 axis = (fromUnit ^ ortho).Normalized();
        return {axis, 0.f};
    }

    const Vec3 halfVec = (fromUnit + toUnit).Normalized();
    const Vec3 axis = fromUnit ^ halfVec;
    return {axis, fromUnit | halfVec};
    
} // FromTo

// ---------------------------------------------------------------------------------------------------------------------

Quat Quat::LookRotation(const Vec3& direction, const Vec3& up)
{
    // Find orthonormal basis vector
    const Vec3 forward = direction.Normalized();
    const Vec3 right = up.Normalized() ^ forward;
    const Vec3 newUp = forward ^ right;

    // From world forward to object forward
    const Quat forwardRot = FromTo(Vec3(0.f, 0.f, 1.f), forward);
    // Find up rotation
    const Vec3 objectUp = forwardRot * Vec3(0.f, 1.f, 0.f);
    const Quat upRot = FromTo(objectUp, newUp);

    // Concatenate rotations, first forward then up
    return (forwardRot * upRot).Normalized();
    
} // LookRotation

// ---------------------------------------------------------------------------------------------------------------------

Quat Quat::Mix(const Quat& from, const Quat& to, float t)
{
    return from + (to - from) * t;
    
} // Mix

// ---------------------------------------------------------------------------------------------------------------------

Quat Quat::Mix(const Quat& to, float t) const
{
    return Mix(*this, to, t);
    
} // Mix

// ---------------------------------------------------------------------------------------------------------------------

Quat Quat::NLerp(const Quat& from, const Quat& to, float t)
{
    return Mix(from, to, t).Normalized();
    
} // NLerp

// ---------------------------------------------------------------------------------------------------------------------

Quat Quat::NLerp(const Quat& to, float t) const
{
    return NLerp(*this, to, t);
    
} // NLerp

// ---------------------------------------------------------------------------------------------------------------------

Quat Quat::SLerp(const Quat& from, const Quat& to, float t)
{
    if (Utils::AreEqual(from | to, 1.f))
    {
        return NLerp(from, to, t);
    }

    const Quat delta = from.Inverse() * to;
    return ((delta ^ t) * from).Normalized();
    
} // SLerp

// ---------------------------------------------------------------------------------------------------------------------

Quat Quat::SLerp(const Quat& to, float t) const
{
    return SLerp(*this, to, t);
    
} // SLerp

// ---------------------------------------------------------------------------------------------------------------------

Mat4 Quat::ToMat4() const
{
    const Vec3 right = *this * Vec3(1.f, 0.f, 0.f);
    const Vec3 up = *this * Vec3(0.f, 1.f, 0.f);
    const Vec3 forward = *this * Vec3(0.f, 0.f, 1.f);

    return
    {
        right.x, right.y, right.z, 0.f,
        up.x, up.y, up.z, 0.f,
        forward.x, forward.y, forward.z, 0.f,
        0.f, 0.f, 0.f, 1.f
    };
    
} // ToMat4

// ---------------------------------------------------------------------------------------------------------------------

Quat Quat::FromMat4(const Mat4& m)
{
    const Vec3 up = Vec3(m.up.x, m.up.y, m.up.z).Normalized();
    const Vec3 forward = Vec3(m.forward.x, m.forward.y, m.forward.z).Normalized();
    const Vec3 right = up ^ forward;
    const Vec3 correctedUp = forward ^ right;

    return LookRotation(forward, correctedUp);
    
} // FromMat4
    
// ---------------------------------------------------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& os, const Quat& q)
{
    os << "(" << q.x << ", " << q.y << ", " << q.z << ", " << q.w << ")";
    return os;
    
} // operator<<

// ---------------------------------------------------------------------------------------------------------------------
