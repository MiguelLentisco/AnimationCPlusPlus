#pragma once

template<typename T>
struct TVec2
{
    T x;
    T y;
    
    TVec2(T x = 0, T y = 0) : x(x), y(y) {}
    TVec2(T* v[2]) : x(v[0]), y(v[1]) {}
    
}; // TVec2

typedef TVec2<float> Vec2;
typedef TVec2<int> IVec2;
