#pragma once

class IndexBuffer;
typedef unsigned int GLenum;

enum class DrawMode
{
    Points,
    LineStrip,
    LineLoop,
    Lines,
    Triangles,
    TriangleStrip,
    TriangleFan
    
}; // DrawMode

class DrawLibrary
{
public:
    static void Draw(const IndexBuffer& indexBuffer, DrawMode mode);
    static void Draw(unsigned int vertexCount, DrawMode mode);
    static void DrawInstanced(const IndexBuffer& indexBuffer, DrawMode mode, unsigned int instanceCount);
    static void DrawInstanced(unsigned int vertexCount, DrawMode mode, unsigned int numInstances);
    static GLenum DrawModeToGLEnum(DrawMode input);

    DrawLibrary() = delete;
    
}; // DrawLibrary