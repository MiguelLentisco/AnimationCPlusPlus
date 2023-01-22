#pragma once
#include <ostream>

struct Vec3
{
    float x;
    float y;
    float z;
    
    Vec3(float x = 0.f, float y = 0.f, float z = 0.f) : x(x), y(y), z(z) {}
    Vec3(const float* v) : x(v[0]), y(v[1]), z(v[2]) {}

    float& operator[](int i);
    const float& operator[](int i) const;

    bool operator==(const Vec3& v) const;
    bool operator!=(const Vec3& v) const;

    Vec3 operator-() const;
    Vec3 operator+(const Vec3& v) const;
    Vec3 operator-(const Vec3& v) const;
    Vec3 operator*(float f) const;
    Vec3 operator*(const Vec3& v) const;
    Vec3 operator/(float f) const;
    Vec3 operator/(const Vec3& v) const;

    Vec3& operator+=(float f);
    Vec3& operator+=(const Vec3& v);
    Vec3& operator-=(float f);
    Vec3& operator-=(const Vec3& v);
    Vec3& operator*=(float f);
    Vec3& operator*=(const Vec3& v);

    bool IsZeroVec() const;

    static float Dot(const Vec3& u, const Vec3& v);
    float Dot(const Vec3& v) const;
    float operator|(const Vec3& v) const;
    static Vec3 Cross(const Vec3& u, const Vec3& v);
    Vec3 Cross(const Vec3& v) const;
    Vec3 operator^(const Vec3& v) const;
    
    float Len() const;
    float LenSq() const;
    static float Dist(const Vec3& u, const Vec3& v);
    float Dist(const Vec3& v) const;
    static float DistSq(const Vec3& u, const Vec3& v);
    float DistSq(const Vec3& v) const;

    void Normalize();
    Vec3 Normalized() const;

    static float Angle(const Vec3& u, const Vec3& v);
    float Angle(const Vec3& v) const;

    static Vec3 Project(const Vec3& u, const Vec3& v);
    Vec3 Project(const Vec3& v) const;
    static Vec3 Reject(const Vec3& u, const Vec3& v);
    Vec3 Reject(const Vec3& v) const;
    static Vec3 Reflect(const Vec3& u, const Vec3& v);
    Vec3 Reflect(const Vec3& v) const;

    static Vec3 Lerp(const Vec3& u, const Vec3& v, float t);
    static Vec3 SLerp(const Vec3& u, const Vec3& v, float t);
    static Vec3 NLerp(const Vec3& u, const Vec3& v, float t);

    friend std::ostream& operator<<(std::ostream& os, const Vec3& v);
    
}; // vec3
