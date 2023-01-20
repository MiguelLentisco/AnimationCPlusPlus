#pragma once

#include <ostream>

#include "Core/Quat.h"
#include "Core/Vec3.h"

struct Transform
{ 
    Vec3 position;
    Quat rotation;
    Vec3 scale;
    
    Transform(const Vec3& pos = {}, const Quat& rot = {}, const Vec3& scale = {1.f, 1.f, 1.f}) :
        position(pos), rotation(rot), scale(scale) {}

    Transform Combine(const Transform& t) const;
    Transform Inverse() const;

    Vec3 TransformPoint(const Vec3& p) const;
    Vec3 TransformVector(const Vec3& v) const;

    Mat4 ToMat4() const;
    static Transform FromMat4(const Mat4& m);

    static Transform Mix(const Transform& from, const Transform& to, float t);
    Transform Mix(const Transform& to, float t) const;

    bool operator ==(const Transform& other) const;
    bool operator !=(const Transform& other) const;

    friend std::ostream& operator<<(std::ostream& os, const Transform& t);
    
}; // Transform
