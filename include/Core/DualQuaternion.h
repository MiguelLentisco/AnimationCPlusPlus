#pragma once
#include "Quat.h"

struct DualQuaternion
{
    union
    {
        struct
        {
            Quat real;
            Quat dual;
        };

        float v[8];
    };
    
    DualQuaternion(const Quat& real = {0, 0, 0, 1}, const Quat& dual = {0, 0, 0, 0}) : real(real), dual(dual) {}

    DualQuaternion operator+(const DualQuaternion& q) const;
    DualQuaternion operator*(float f) const;
    DualQuaternion operator*(const DualQuaternion& q) const;
    DualQuaternion& operator*=(float f);
    DualQuaternion& operator*=(const DualQuaternion& q);
    float operator|(const DualQuaternion& q) const;

    static float Dot(const DualQuaternion& q1, const DualQuaternion& q2);
    float Dot(const DualQuaternion& q) const;

    DualQuaternion Conjugated() const;
    void Conjugate();
    DualQuaternion Normalized() const;
    void Normalize();

    Vec3 TransformVector(const Vec3& v) const;
    Vec3 TransformPoint(const Vec3& v) const;

    bool operator==(const DualQuaternion& q) const;
    bool operator!=(const DualQuaternion& q) const;
    
}; // DualQuaternion
