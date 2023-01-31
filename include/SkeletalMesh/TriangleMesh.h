#pragma once

#include "Core/Vec3.h"

struct TriangleMesh
{
    Vec3 v0;
    Vec3 v1;
    Vec3 v2;
    Vec3 normal;

    TriangleMesh() = default;
    TriangleMesh(const Vec3& v0, const Vec3& v1, const Vec3& v2) : v0(v0), v1(v1), v2(v2),
        normal(Vec3::Cross(v1 - v0, v2 - v0).Normalized()) { }
    
}; // TriangleMesh
