#pragma once

#define EPS 0.000001f
#define PI 3.14159265358979f

#include <algorithm>

class BasicUtils
{
public:
    BasicUtils() = delete;
    BasicUtils(const BasicUtils&) = delete;
    BasicUtils& operator=(const BasicUtils&) = delete;
    
    static bool AreEqual(float a, float b, float tol = EPS);
    static bool IsZero(float a, float tol = EPS);

    static float DegToRad(float deg);
    static float RadToDef(float rad);

    template <typename T>
    static T Clamp(const T& val, const T& lower, const T& upper)
    {
        return std::max(lower, std::min(val, upper));
    
    } // Clamp

    template <typename T>
    static T Lerp(const T& start, const T& end, float alpha)
    {
        return start * (1.f - alpha) + end * alpha;
    
    } // Clamp

    static int FloatToInt(float f);
    
}; // BasicUtils

