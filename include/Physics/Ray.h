#pragma once

#include "Core/Vec3.h"

struct Ray
{
    Vec3 origin;
    Vec3 direction;
    
    Ray(const Vec3& origin = {}, const Vec3& direction = {0, -1, 0}) : origin(origin), direction(direction) {}
    
}; // Ray
