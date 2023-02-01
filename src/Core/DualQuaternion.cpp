#include "Core/DualQuaternion.h"

#include "Core/BasicUtils.h"

// ---------------------------------------------------------------------------------------------------------------------

DualQuaternion DualQuaternion::operator+(const DualQuaternion& q) const
{
    return {real + q.real, dual + q.dual};
    
} // operator+

// ---------------------------------------------------------------------------------------------------------------------

DualQuaternion DualQuaternion::operator*(float f) const
{
    return {real * f, dual * f};
    
} // operator*

// ---------------------------------------------------------------------------------------------------------------------

DualQuaternion DualQuaternion::operator*(const DualQuaternion& q) const
{
    const DualQuaternion leftNorm = this->Normalized();
    const DualQuaternion rightNorm = q.Normalized();
    
    return {leftNorm.real * rightNorm.real, leftNorm.real * rightNorm.dual + leftNorm.dual * rightNorm.real};
    
} // operator*

// ---------------------------------------------------------------------------------------------------------------------

DualQuaternion& DualQuaternion::operator*=(float f)
{
    real *= f;
    dual *= f;

    return *this;
    
} // operator*=

// ---------------------------------------------------------------------------------------------------------------------

DualQuaternion& DualQuaternion::operator*=(const DualQuaternion& q)
{
    Normalize();
    const DualQuaternion rightNorm = q.Normalized();

    dual *= rightNorm.real;
    dual += real * rightNorm.dual;
    real *= rightNorm.real;

    return *this;
    
} // operator*=

// ---------------------------------------------------------------------------------------------------------------------

float DualQuaternion::operator|(const DualQuaternion& q) const
{
    return real | q.real;
    
} // operator|

// ---------------------------------------------------------------------------------------------------------------------

float DualQuaternion::Dot(const DualQuaternion& q1, const DualQuaternion& q2)
{
    return q1 | q2;
    
} // Dot

// ---------------------------------------------------------------------------------------------------------------------

float DualQuaternion::Dot(const DualQuaternion& q) const
{
    return *this | q;
    
} // Dot

// ---------------------------------------------------------------------------------------------------------------------

DualQuaternion DualQuaternion::Conjugated() const
{
    return {real.Conjugate(), dual.Conjugate()};
    
} // Conjugated

// ---------------------------------------------------------------------------------------------------------------------

void DualQuaternion::Conjugate()
{
    real.vector = -real.vector;
    dual.vector = -dual.vector;
    
} // Conjugate

// ---------------------------------------------------------------------------------------------------------------------

DualQuaternion DualQuaternion::Normalized() const
{
    const float lenSq = real | real;
    if (BasicUtils::IsZero(lenSq))
    {
        return {};
    }

    const float lenInv = 1.f / std::sqrtf(lenSq);
    return {real * lenInv, dual * lenInv};
    
} // Normalized

// ---------------------------------------------------------------------------------------------------------------------

void DualQuaternion::Normalize()
{
    const float lenSq = real | real;
    if (BasicUtils::IsZero(lenSq))
    {
        return;
    }
    
    const float lenInv = 1.f / std::sqrtf(lenSq);
    real *= lenInv;
    dual *= lenInv;
    
} // Normalize

// ---------------------------------------------------------------------------------------------------------------------

Vec3 DualQuaternion::TransformVector(const Vec3& v) const
{
    return real * v;
    
} // TransformVector

// ---------------------------------------------------------------------------------------------------------------------

Vec3 DualQuaternion::TransformPoint(const Vec3& v) const
{
    const Quat position = real.Conjugate() * (dual * 2.f);

    return real * v + position.vector;
    
} // TransformPoint

// ---------------------------------------------------------------------------------------------------------------------

bool DualQuaternion::operator==(const DualQuaternion& q) const
{
    return real == q.real && dual == q.dual;
    
} // operator==

// ---------------------------------------------------------------------------------------------------------------------

bool DualQuaternion::operator!=(const DualQuaternion& q) const
{
    return !(*this == q);
    
} // operator!=

// ---------------------------------------------------------------------------------------------------------------------
