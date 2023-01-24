#include "Render/DebugDrawer.h"

#include "Core/Transform.h"
#include "Core/Vec3.h"
#include "Render/Attribute.h"
#include "Render/Draw.h"
#include "Render/Shader.h"
#include "Render/Uniform.h"
#include "SkeletalMesh/Pose.h"

// ---------------------------------------------------------------------------------------------------------------------

DebugDrawer::DebugDrawer(unsigned size)
{
    static const std::string VERTEX_CODE =
        "#version 330 core\n"
        "uniform mat4 mvp;\n"
        "in vec3 position;\n"
        "void main() {\n"
        "	gl_Position = mvp * vec4(position, 1.0);\n"
        "}";
    static const std::string FRAG_CODE =
        "#version 330 core\n"
        "uniform vec3 color;\n"
        "out vec4 FragColor;\n"
        "void main() {\n"
        "	FragColor = vec4(color, 1);\n"
        "}";
    m_Shader = new Shader(VERTEX_CODE, FRAG_CODE);
    
    m_Attributes = new Attribute<Vec3>();
    m_Points.resize(size);
    
} // DebugDrawer

// ---------------------------------------------------------------------------------------------------------------------

DebugDrawer::~DebugDrawer()
{
    delete m_Attributes;
    delete m_Shader;
    
} // DebugDrawer

// ---------------------------------------------------------------------------------------------------------------------

Vec3& DebugDrawer::operator[](unsigned index)
{
    return m_Points[index];
    
} // operator[]

// ---------------------------------------------------------------------------------------------------------------------

const Vec3& DebugDrawer::operator[](unsigned index) const
{
    return m_Points[index];
    
} // operator[]

// ---------------------------------------------------------------------------------------------------------------------

void DebugDrawer::Push(const Vec3& v)
{
    m_Points.push_back(v);
    
} // Push

// ---------------------------------------------------------------------------------------------------------------------

void DebugDrawer::UpdateOpenGLBuffers()
{
    if (!m_Points.empty())
    {
        m_Attributes->Set(m_Points); 
    }
    
} // UpdateOpenGLBuffers

// ---------------------------------------------------------------------------------------------------------------------

void DebugDrawer::Draw(DebugDrawMode mode, const Vec3& color, const Mat4& mvp)
{
    m_Shader->Bind();
    Uniform<Mat4>::Set(m_Shader->GetUniform("mvp"), mvp);
    Uniform<Vec3>::Set(m_Shader->GetUniform("color"), color);
    m_Attributes->BindTo(m_Shader->GetAttribute("position"));

    DrawMode drawMode;
    switch (mode)
    {
        case DebugDrawMode::Lines:
            drawMode = DrawMode::Lines;
            break;
        case DebugDrawMode::Loop:
            drawMode = DrawMode::LineLoop;
            break;
        case DebugDrawMode::Strip:
            drawMode = DrawMode::LineStrip;
            break;
        case DebugDrawMode::Points:
        default:
            drawMode = DrawMode::Points;
    }

    DrawLibrary::Draw(m_Points.size(), drawMode);
    
    m_Attributes->UnbindFrom(m_Shader->GetAttribute("position"));
    m_Shader->Unbind();
    
} // Draw

// ---------------------------------------------------------------------------------------------------------------------

void DebugDrawer::FromPose(const Pose& pose)
{
    unsigned int requiredVerts = 0;
    const unsigned int numJoints = pose.GetSize();
    for (unsigned int i = 0; i < numJoints; ++i)
    {
        if (pose.GetParent(i) < 0)
        {
            continue;
        }
        
        requiredVerts += 2;
    }

    m_Points.resize(requiredVerts);
    for (unsigned int i = 0; i < numJoints; ++i)
    {
        if (pose.GetParent(i) < 0)
        {
            continue;
        }

        const unsigned int idx = 2 * i;
        m_Points[idx] = pose.GetGlobalTransform(i).position;
        m_Points[idx + 1] = pose.GetGlobalTransform(pose.GetParent(i)).position;
    }
    
} // FromPose

// ---------------------------------------------------------------------------------------------------------------------
