#pragma once

#include <vector>

template<typename T>
class Attribute
{
public:
    Attribute();
    ~Attribute();
    
    Attribute(const Attribute& other) = delete;
    Attribute& operator=(const Attribute& other) = delete;

    unsigned int GetHandle() const;
    unsigned int GetCount() const;

    void Set(const T* inputArray, unsigned int arrayLength);
    void Set(const std::vector<T>& input);

    void BindTo(unsigned int slot);
    void UnbindFrom(unsigned int slot) const;

protected:
    unsigned int Handle;
    unsigned int Count;

private:
    void SetAttribPointer(unsigned int slot);
    
}; // Attribute


