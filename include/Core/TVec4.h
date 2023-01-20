#pragma once

template<typename T>
struct TVec4
{
    T x;
    T y;
    T z;
    T w;
    
    TVec4(T x = 0, T y = 0, T z = 0, T w = 0) : x(x), y(y), z(z), w(w) {}
    TVec4(T* v[4]) : x(v[0]), y(v[1]), z(v[2]), w(v[3]) {}
    
}; // TVec2

typedef TVec4<float> Vec4;
typedef TVec4<int> IVec4;
typedef TVec4<unsigned int> UIVec4;