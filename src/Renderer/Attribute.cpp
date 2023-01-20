#include "Renderer/Attribute.h"

#include "Core/TVec2.h"
#include "Core/Vec3.h"
#include "Core/TVec4.h"
#include "glad/glad.h"
#include <vector>

// ---------------------------------------------------------------------------------------------------------------------

template Attribute<int>;
template Attribute<float>;
template Attribute<Vec2>;
template Attribute<Vec3>;
template Attribute<Vec4>;
template Attribute<IVec4>;

// ---------------------------------------------------------------------------------------------------------------------

template <typename T>
Attribute<T>::Attribute() : Count(0)
{
    glGenBuffers(1, &Handle);
    
} // Attribute

// ---------------------------------------------------------------------------------------------------------------------

template <typename T>
Attribute<T>::~Attribute()
{
    glDeleteBuffers(1, &Handle);
    
} // ~Attribute

// ---------------------------------------------------------------------------------------------------------------------

template <typename T>
unsigned Attribute<T>::GetHandle() const
{
    return Handle;
    
} // GetHandle

// ---------------------------------------------------------------------------------------------------------------------

template <typename T>
unsigned Attribute<T>::GetCount() const
{
    return Count;
    
} // GetCount

// ---------------------------------------------------------------------------------------------------------------------

template <typename T>
void Attribute<T>::Set(const T* inputArray, unsigned arrayLength)
{
    Count = arrayLength;
    static unsigned int CLASS_SIZE = sizeof(T);

    glBindBuffer(GL_ARRAY_BUFFER, Handle);
    glBufferData(GL_ARRAY_BUFFER, CLASS_SIZE * Count, inputArray, GL_STREAM_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
} // Set

// ---------------------------------------------------------------------------------------------------------------------

template <typename T>
void Attribute<T>::Set(const std::vector<T>& input)
{
    Set(&input[0], static_cast<unsigned>(input.size()));
    
} // Set

// ---------------------------------------------------------------------------------------------------------------------

template <typename T>
void Attribute<T>::BindTo(unsigned slot)
{
    glBindBuffer(GL_ARRAY_BUFFER, Handle);
    glEnableVertexAttribArray(slot);
    SetAttribPointer(slot);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
} // BindTo

// ---------------------------------------------------------------------------------------------------------------------

template <typename T>
void Attribute<T>::UnbindFrom(unsigned slot) const
{
    glBindBuffer(GL_ARRAY_BUFFER, Handle);
    glDisableVertexAttribArray(slot);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
} // UnbindFrom

// ---------------------------------------------------------------------------------------------------------------------

template<>
void Attribute<int>::SetAttribPointer(unsigned slot)
{
    glVertexAttribIPointer(slot, 1, GL_INT, 0, nullptr);
    
} // SetAttribPointer

// ---------------------------------------------------------------------------------------------------------------------

template<>
void Attribute<IVec4>::SetAttribPointer(unsigned slot)
{
    glVertexAttribIPointer(slot, 4, GL_INT, 0, nullptr);
    
} // SetAttribPointer

// ---------------------------------------------------------------------------------------------------------------------

template<>
void Attribute<float>::SetAttribPointer(unsigned slot)
{
    glVertexAttribPointer(slot, 1, GL_FLOAT, GL_FALSE, 0, nullptr);
    
} // SetAttribPointer

// ---------------------------------------------------------------------------------------------------------------------

template<>
void Attribute<Vec2>::SetAttribPointer(unsigned slot)
{
    glVertexAttribPointer(slot, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    
} // SetAttribPointer

// ---------------------------------------------------------------------------------------------------------------------

template<>
void Attribute<Vec3>::SetAttribPointer(unsigned slot)
{
    glVertexAttribPointer(slot, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    
} // SetAttribPointer

// ---------------------------------------------------------------------------------------------------------------------

template<>
void Attribute<Vec4>::SetAttribPointer(unsigned slot)
{
    glVertexAttribPointer(slot, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
    
} // SetAttribPointer

// ---------------------------------------------------------------------------------------------------------------------