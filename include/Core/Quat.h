#pragma once

#include "Core/Vec3.h"

struct Mat4;

struct Quat
{
    union
    {
        struct
        {
            float x;
            float y;
            float z;
            float w;
        };

        struct
        {
            Vec3 vector;
            float scalar;
        };
        
        float v[4];
    };

    Quat(float x = 0.f, float y = 0.f, float z = 0.f, float w = 1.f) : x(x), y(y), z(z), w(w) {}
    Quat(const Vec3& vector, float scalar) : vector(vector), scalar(scalar) {}
    Quat(const float v[4]) : Quat(v[0], v[1], v[2], v[3]) {}

    Vec3 GetAxis() const;
    float GetAngle() const;

    float LenSq() const;
    float Len() const;

    void Normalize();
    Quat Normalized() const;

    Quat Conjugate() const;
    Quat Inverse() const;

    Quat operator-() const;
    Quat operator+(const Quat& q) const;
    Quat operator-(const Quat& q) const;
    Quat operator*(float f) const;
    Quat operator*(const Quat& q) const;
    Vec3 operator*(const Vec3& v) const;

    Quat& operator+=(const Quat& q);
    Quat& operator*=(float f);
    Quat& operator*=(const Quat& q);

    float operator|(const Quat& q) const;
    static float Dot(const Quat& q1, const Quat& q2);
    float Dot(const Quat& q) const;

    Quat operator^(float f) const;

    bool operator==(const Quat& q) const;
    bool operator!=(const Quat& q) const;

    static bool WithSameOrientation(const Quat& q1, const Quat& q2);
    bool WithSameOrientation(const Quat& q) const;

    static Quat GetNeighbour(const Quat& q, const Quat& other);
    Quat GetNeighbour(const Quat& other) const;

    static Quat CreateFromAxis(float angle, const Vec3& axis);
    static Quat FromTo(const Vec3& from, const Vec3& to);
    static Quat LookRotation(const Vec3& direction, const Vec3& up);
    
    static Quat Mix(const Quat& from, const Quat& to, float t);
    Quat Mix(const Quat& to, float t) const;
    static Quat NLerp(const Quat& from, const Quat& to, float t);
    Quat NLerp(const Quat& to, float t) const;
    static Quat SLerp(const Quat& from, const Quat& to, float t);
    Quat SLerp(const Quat& to, float t) const;

    Mat4 ToMat4() const;
    static Quat FromMat4(const Mat4& m);

    friend std::ostream& operator<<(std::ostream& os, const Quat& q);

}; // Quat
