#pragma once

struct Vec3;
struct TriangleMesh;
struct Ray;

class PhysicsLibrary
{
public:
    PhysicsLibrary() = delete;

    static bool RaycastTriangle(const Ray& ray, const TriangleMesh& triangle, Vec3& hitPoint);
    
}; // PhysicsLibrary
