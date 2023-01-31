#include "Physics/PhysicsLibrary.h"

#include "Core/BasicUtils.h"
#include "Physics/Ray.h"
#include "SkeletalMesh/TriangleMesh.h"

// ---------------------------------------------------------------------------------------------------------------------

bool PhysicsLibrary::RaycastTriangle(const Ray& ray, const TriangleMesh& triangle, Vec3& hitPoint)
{
    static constexpr float EPSILON = 0.0000001f;
    
    const Vec3 edge10 = triangle.v1 - triangle.v0;
    const Vec3 edge20 = triangle.v2 - triangle.v0;
    const Vec3 h = ray.direction ^ edge20;
    const float a = edge10 | h;
    
    if (BasicUtils::IsZero(a, EPSILON))
    {
        return false;
    }

    const float aInv = 1.0f / a;
    const Vec3 s = ray.origin - triangle.v0;
    const float u = aInv * (s | h);
    
    if (u < 0.0f || u > 1.0f)
    {
        return false;
    }
       
    const Vec3 q = s ^ edge10;
    const float v = aInv * (ray.direction | q);
    if (v < 0.0f || u + v > 1.0f)
    {
        return false;
    }
        
    const float t = aInv * (edge20 | q);
    if (t < EPSILON)
    {
        return false;
    }
    
    hitPoint = ray.origin + ray.direction * t;
    return true;
    
} // RaycastTriangle

// ---------------------------------------------------------------------------------------------------------------------

