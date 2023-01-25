#include "Core/BasicUtils.h"

#include <cmath>

// ---------------------------------------------------------------------------------------------------------------------

bool BasicUtils::AreEqual(float a, float b, float tol)
{
    return std::abs(a - b) <= tol; 
    
} // AreEqual

// ---------------------------------------------------------------------------------------------------------------------

bool BasicUtils::IsZero(float a, float tol)
{
    return AreEqual(a, 0.f, tol);
    
} // IsZero

// ---------------------------------------------------------------------------------------------------------------------

float BasicUtils::DegToRad(float deg)
{
    static constexpr float DEG_TO_RAD = PI / 180.f;
    return deg * DEG_TO_RAD;
    
} // DegToRad

// ---------------------------------------------------------------------------------------------------------------------

float BasicUtils::RadToDef(float rad)
{
    static constexpr float RAD_TO_DEG = 180.f / PI;
    return rad * RAD_TO_DEG;
    
} // RadToDef

// ---------------------------------------------------------------------------------------------------------------------

int BasicUtils::FloatToInt(float f)
{
    return static_cast<int>(std::round(f) + EPS);
    
} // FloatToInt

// ---------------------------------------------------------------------------------------------------------------------
