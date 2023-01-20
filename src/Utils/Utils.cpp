#include "Utils/Utils.h"

#include <cmath>

// ---------------------------------------------------------------------------------------------------------------------

bool Utils::AreEqual(float a, float b, float tol)
{
    return std::abs(a - b) <= tol; 
    
} // AreEqual

// ---------------------------------------------------------------------------------------------------------------------

bool Utils::IsZero(float a, float tol)
{
    return AreEqual(a, 0.f, tol);
    
} // IsZero

// ---------------------------------------------------------------------------------------------------------------------

float Utils::DegToRad(float deg)
{
    static constexpr float DEG_TO_RAD = PI / 180.f;
    return deg * DEG_TO_RAD;
    
} // DegToRad

// ---------------------------------------------------------------------------------------------------------------------

float Utils::RadToDef(float rad)
{
    static constexpr float RAD_TO_DEG = 180.f / PI;
    return rad * RAD_TO_DEG;
    
} // RadToDef

// ---------------------------------------------------------------------------------------------------------------------