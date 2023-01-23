#pragma once

#define EPS 0.000001f
#define PI 3.14159265358979f

#include <algorithm>

class Utils
{
public:
    static bool AreEqual(float a, float b, float tol = EPS);
    static bool IsZero(float a, float tol = EPS);

    static float DegToRad(float deg);
    static float RadToDef(float rad);

    template <typename T>
    static T Clamp(const T& n, const T& lower, const T& upper)
    {
        return std::max(lower, std::min(n, upper));
    
    } // Clamp

    static int FloatToInt(float f);
    
}; // Utils

