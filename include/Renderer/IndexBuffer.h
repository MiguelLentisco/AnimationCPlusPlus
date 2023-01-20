#pragma once

#include <vector>

class IndexBuffer
{
public:
    IndexBuffer();
    ~IndexBuffer();
    
    IndexBuffer(const IndexBuffer& other) = delete;
    IndexBuffer& operator=(const IndexBuffer& other) = delete;
    
    unsigned int GetHandle() const;
    unsigned int GetCount() const;
    
    void Set(const unsigned int* inputArray, unsigned int arrayLength);
    void Set(const std::vector<unsigned int>& inputArray);

private:
    unsigned int Handle;
    unsigned int Count;
    
}; // IndexBuffer
