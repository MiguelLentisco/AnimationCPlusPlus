#pragma once

#include "Application.h"

class Texture;
class IndexBuffer;
class Shader;
struct Vec3;
template<typename T> struct TVec2;
typedef TVec2<float> Vec2;
template<typename T> class Attribute;

class BasicRenderApp : public Application
{
public:
    void Initialize() override;
    void Update(float inDeltaTime) override;
    void Render(float inAspectRatio) override;
    void Shutdown() override;

protected:
    Shader* m_Shader = nullptr;
    Attribute<Vec3>* m_VertexPositions = nullptr;
    Attribute<Vec3>* m_VertexNormals = nullptr;
    Attribute<Vec2>* m_VertexTexCoords = nullptr;
    IndexBuffer* m_IndexBuffer = nullptr;
    Texture* m_DisplayTexture = nullptr;
    float m_Rotation = 0.f;
    
}; // BasicRenderApp
