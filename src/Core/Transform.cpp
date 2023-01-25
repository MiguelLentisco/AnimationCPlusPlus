#include "Core/Transform.h"

#include "Core/BasicUtils.h"
#include "Core/Mat4.h"

// ---------------------------------------------------------------------------------------------------------------------

Transform Transform::Combine(const Transform& t) const
{
    const Vec3 resScale = scale * t.scale;
    const Quat resRot = t.rotation * rotation;
    const Vec3 resPos = position + rotation * (scale * t.position);
    
    return {resPos, resRot, resScale};
    
} // Combine

// ---------------------------------------------------------------------------------------------------------------------

Transform Transform::Inverse() const
{
    const Quat invRot = rotation.Inverse();

    auto InvFloat = [](float f) -> float { return BasicUtils::IsZero(f) ? 0.f : 1.f / f; };
    const Vec3 invScale = {InvFloat(scale.x), InvFloat(scale.y), InvFloat(scale.z)}; 

    const Vec3 invPos = invRot * (invScale * -position);

    return {invPos, invRot, invScale};
    
} // Inverse

// ---------------------------------------------------------------------------------------------------------------------

Vec3 Transform::TransformPoint(const Vec3& p) const
{
    return position + rotation * (scale * p);
    
} // TransformPoint

// ---------------------------------------------------------------------------------------------------------------------

Vec3 Transform::TransformVector(const Vec3& v) const
{
    return rotation * (scale * v);
    
} // TransformVector

// ---------------------------------------------------------------------------------------------------------------------

Mat4 Transform::ToMat4() const
{
    // Rotate and scale basis
    const Vec3 right = rotation * Vec3(1.f, 0.f, 0.f) * scale.x;
    const Vec3 up = rotation * Vec3(0.f, 1.f, 0.f) * scale.y;
    const Vec3 forward = rotation * Vec3(0.f, 0.f, 1.f) * scale.z;

    return{
        right.x, right.y, right.z, 0.f,
        up.x, up.y, up.z, 0.f,
        forward.x, forward.y, forward.z, 0.f,
        position.x, position.y, position.z, 1.f
    };
    
} // ToMat4

// ---------------------------------------------------------------------------------------------------------------------

Transform Transform::FromMat4(const Mat4& m)
{
    const Vec3 position = {m.v[12], m.v[13], m.v[14]};
    const Quat rotation = Quat::FromMat4(m);

    Mat4 rotScaleMat = {
        m.v[0], m.v[1], m.v[2], 0.f,
        m.v[4], m.v[5], m.v[6], 0.f,
        m.v[8], m.v[9], m.v[10], 0.f,
        0.f, 0.f, 0.f, 1.f
    };

    const Mat4 inverseRotMat = rotation.Inverse().ToMat4();
    const Mat4 scaleSkewMat = rotScaleMat * inverseRotMat;
    const Vec3 scale = {scaleSkewMat.v[0], scaleSkewMat.v[5], scaleSkewMat.v[10]};

    return {position, rotation, scale};
    
} // FromMat4

// ---------------------------------------------------------------------------------------------------------------------

Transform Transform::Mix(const Transform& from, const Transform& to, float t)
{
    // Select the rot with minimal path
    const Quat correctedRot = (from.rotation | to.rotation) < 0.f ? -to.rotation : to.rotation;

    return{
        Vec3::Lerp(from.position, to.position, t),
        Quat::NLerp(from.rotation, correctedRot, t),
        Vec3::Lerp(from.scale, to.scale, t)
    };
    
} // Mix

// ---------------------------------------------------------------------------------------------------------------------

Transform Transform::Mix(const Transform& to, float t) const
{
    return Mix(*this, to, t);
    
} // Mix

// ---------------------------------------------------------------------------------------------------------------------

bool Transform::operator==(const Transform& other) const
{
    if (position != other.position)
    {
        return false;
    }

    return position == other.position && scale == other.scale && rotation == other.rotation;
    
} // operator==

// ---------------------------------------------------------------------------------------------------------------------

bool Transform::operator!=(const Transform& other) const
{
    return !(*this == other);
    
} // operator!=

// ---------------------------------------------------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& os, const Transform& t)
{
    os << "Position = " << t.position << " | Rotation = " << t.rotation << " | Scale = " << t.scale;
    return os;
    
} // operator<<

// ---------------------------------------------------------------------------------------------------------------------
