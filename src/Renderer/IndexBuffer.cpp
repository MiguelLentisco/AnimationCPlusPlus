#include "Renderer/IndexBuffer.h"

#include "glad/glad.h"

// ---------------------------------------------------------------------------------------------------------------------

IndexBuffer::IndexBuffer()
{
    glGenBuffers(1, &Handle);
    Count = 0;
    
} // IndexBuffer

// ---------------------------------------------------------------------------------------------------------------------

IndexBuffer::~IndexBuffer()
{
    glDeleteBuffers(1, &Handle);
    
} // ~IndexBuffer

// ---------------------------------------------------------------------------------------------------------------------

unsigned IndexBuffer::GetHandle() const
{
    return Handle;
    
} // GetHandle

// ---------------------------------------------------------------------------------------------------------------------

unsigned IndexBuffer::GetCount() const
{
    return Count;
    
} // GetCount

// ---------------------------------------------------------------------------------------------------------------------

void IndexBuffer::Set(const unsigned* inputArray, unsigned arrayLength)
{
    Count = arrayLength;
    static constexpr unsigned int SIZE = sizeof(unsigned int);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Handle);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, SIZE * Count, inputArray, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
} // Set

// ---------------------------------------------------------------------------------------------------------------------

void IndexBuffer::Set(const std::vector<unsigned>& inputArray)
{
    Set(inputArray.data(), inputArray.size());
    
} // Set

// ---------------------------------------------------------------------------------------------------------------------
