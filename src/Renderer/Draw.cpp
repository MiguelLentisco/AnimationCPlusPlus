#include "Renderer/Draw.h"

#include "Renderer/IndexBuffer.h"
#include "glad/glad.h"
#include <iostream>

// ---------------------------------------------------------------------------------------------------------------------

void DrawLibrary::Draw(const IndexBuffer& indexBuffer, DrawMode mode)
{
    const unsigned int handle = indexBuffer.GetHandle();
    const unsigned int numIndex = indexBuffer.GetCount();

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle);
    glDrawElements(DrawModeToGLEnum(mode), static_cast<GLsizei>(numIndex), GL_UNSIGNED_INT, nullptr);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
} // Draw

// ---------------------------------------------------------------------------------------------------------------------

void DrawLibrary::Draw(unsigned vertexCount, DrawMode mode)
{
    glDrawArrays(DrawModeToGLEnum(mode), 0, static_cast<GLsizei>(vertexCount));
    
} // Draw

// ---------------------------------------------------------------------------------------------------------------------

void DrawLibrary::DrawInstanced(const IndexBuffer& indexBuffer, DrawMode mode, unsigned instanceCount)
{
    const unsigned int handle = indexBuffer.GetHandle();
    const unsigned int numIndex = indexBuffer.GetCount();

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle);
    glDrawElementsInstanced(DrawModeToGLEnum(mode), static_cast<GLsizei>(numIndex), GL_UNSIGNED_INT, nullptr,
        static_cast<GLsizei>(instanceCount));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
} // DrawInstanced

// ---------------------------------------------------------------------------------------------------------------------

void DrawLibrary::DrawInstanced(unsigned vertexCount, DrawMode mode, unsigned numInstances)
{
    glDrawArraysInstanced(DrawModeToGLEnum(mode), 0, static_cast<GLsizei>(vertexCount),
        static_cast<GLsizei>(numInstances));
    
} // DrawInstanced

// ---------------------------------------------------------------------------------------------------------------------

GLenum DrawLibrary::DrawModeToGLEnum(DrawMode input)
{
    switch (input)
    {
        case DrawMode::Points: return GL_POINTS;
        case DrawMode::LineStrip: return GL_LINE_STRIP;
        case DrawMode::LineLoop: return GL_LINE_LOOP;
        case DrawMode::Lines: return GL_LINES;
        case DrawMode::Triangles: return GL_TRIANGLES;
        case DrawMode::TriangleStrip: return GL_TRIANGLE_STRIP;
        case DrawMode::TriangleFan: return GL_TRIANGLE_FAN;
    }

    std::cout << "DrawModeToGLEnum unreachable code hit" << std::endl;
    return 0;
    
} // DrawModeToGLEnum

// ---------------------------------------------------------------------------------------------------------------------