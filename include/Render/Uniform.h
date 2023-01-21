#pragma once

#include <vector>

template <typename T>
class Uniform
{
public:
    Uniform() = delete;
    Uniform(const Uniform&) = delete;
    Uniform& operator=(const Uniform&) = delete;
    ~Uniform() = delete;
    
    static void Set(unsigned int slot, const T& value);
    static void Set(unsigned int slot, const T* arr, unsigned int len);
    static void Set(unsigned int slot, const std::vector<T>& arr);
    
}; // Uniform

