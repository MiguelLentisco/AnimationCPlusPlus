#include "SkeletalMesh/SkeletalMesh.h"

#include "Core/Mat4.h"
#include "Core/Transform.h"
#include "Core/TVec2.h"
#include "Core/TVec4.h"
#include "Core/Vec3.h"
#include "Render/Attribute.h"
#include "Render/Draw.h"
#include "Render/IndexBuffer.h"
#include "SkeletalMesh/Skeleton.h"

// ---------------------------------------------------------------------------------------------------------------------

SkeletalMesh::SkeletalMesh() : m_PositionAttribute(new Attribute<Vec3>()), m_NormalAttribute(new Attribute<Vec3>()),
    m_UVAttribute(new Attribute<Vec2>()), m_BonesWeightAttribute(new Attribute<Vec4>()),
    m_BonesIDAttribute(new Attribute<IVec4>()), m_IndexBuffer(new IndexBuffer())
{
    
} // SkeletalMesh

// ---------------------------------------------------------------------------------------------------------------------

SkeletalMesh::SkeletalMesh(const SkeletalMesh& other) : m_Position(other.m_Position), m_Normal(other.m_Normal),
    m_TexCoords(other.m_TexCoords), m_BonesWeight(other.m_BonesWeight), m_BonesID(other.m_BonesID),
    m_Indices(other.m_Indices), m_PositionAttribute(new Attribute<Vec3>()), m_NormalAttribute(new Attribute<Vec3>()),
    m_UVAttribute(new Attribute<Vec2>()), m_BonesWeightAttribute(new Attribute<Vec4>()),
    m_BonesIDAttribute(new Attribute<IVec4>()), m_IndexBuffer(new IndexBuffer())
{
    UpdateOpenGLBuffers();
    
} // SkeletalMesh

// ---------------------------------------------------------------------------------------------------------------------

SkeletalMesh& SkeletalMesh::operator=(const SkeletalMesh& other)
{
    if (this == &other)
    {
        return *this;
    }
    
    m_Position = other.m_Position;
    m_Normal = other.m_Normal;
    m_TexCoords = other.m_TexCoords;
    m_BonesWeight = other.m_BonesWeight;
    m_BonesID = other.m_BonesID;
    m_Indices = other.m_Indices;
    UpdateOpenGLBuffers();

    return *this;
    
} // operator=

// ---------------------------------------------------------------------------------------------------------------------

SkeletalMesh::~SkeletalMesh()
{
    delete m_PositionAttribute;
    delete m_NormalAttribute;
    delete m_UVAttribute;
    delete m_BonesWeightAttribute;
    delete m_BonesIDAttribute;
    delete m_IndexBuffer;
    
} // SkeletalMesh

// ---------------------------------------------------------------------------------------------------------------------

void SkeletalMesh::UpdateOpenGLBuffers()
{
    if (!m_Position.empty())
    {
        m_PositionAttribute->Set(m_Position);
    }
    
    if (!m_Normal.empty())
    {
        m_NormalAttribute->Set(m_Normal);
    }

    if (!m_TexCoords.empty())
    {
        m_UVAttribute->Set(m_TexCoords);
    }

    if (!m_BonesWeight.empty())
    {
        m_BonesWeightAttribute->Set(m_BonesWeight);
    }

    if (!m_BonesID.empty())
    {
        m_BonesIDAttribute->Set(m_BonesID);
    }

    if (!m_Indices.empty())
    {
        m_IndexBuffer->Set(m_Indices);
    }
    
} // UpdateOpenGLBuffers

// ---------------------------------------------------------------------------------------------------------------------

void SkeletalMesh::Bind(int position, int normal, int texCoord, int boneWeight, int boneID)
{
    if (position >= 0)
    {
        m_PositionAttribute->BindTo(position);
    }

    if (normal >= 0)
    {
        m_NormalAttribute->BindTo(normal);
    }

    if (texCoord >= 0)
    {
        m_UVAttribute->BindTo(texCoord);
    }
    
    if (boneWeight >= 0)
    {
        m_BonesWeightAttribute->BindTo(boneWeight);
    }

    if (boneID >= 0)
    {
        m_BonesIDAttribute->BindTo(boneID);
    }
    
} // Bind

// ---------------------------------------------------------------------------------------------------------------------

void SkeletalMesh::Unbind(int position, int normal, int texCoord, int boneWeight, int boneID)
{
    if (position >= 0)
    {
        m_PositionAttribute->UnbindFrom(position);
    }

    if (normal >= 0)
    {
        m_NormalAttribute->UnbindFrom(normal);
    }

    if (texCoord >= 0)
    {
        m_UVAttribute->UnbindFrom(texCoord);
    }
    
    if (boneWeight >= 0)
    {
        m_BonesWeightAttribute->UnbindFrom(boneWeight);
    }

    if (boneID >= 0)
    {
        m_BonesIDAttribute->UnbindFrom(boneID);
    }
    
} // Unbind

// ---------------------------------------------------------------------------------------------------------------------

void SkeletalMesh::Draw()
{
    if (m_Indices.empty())
    {
        DrawLibrary::Draw(m_Position.size(), DrawMode::Triangles);
    }
    else
    {
        DrawLibrary::Draw(*m_IndexBuffer, DrawMode::Triangles);
    }
    
} // Draw

// ---------------------------------------------------------------------------------------------------------------------

void SkeletalMesh::DrawInstanced(unsigned numInstances)
{
    if (m_Indices.empty())
    {
        DrawLibrary::DrawInstanced(m_Position.size(), DrawMode::Triangles, numInstances);
    }
    else
    {
        DrawLibrary::DrawInstanced(*m_IndexBuffer, DrawMode::Triangles, numInstances);
    }
    
} // DrawInstanced

// ---------------------------------------------------------------------------------------------------------------------

void SkeletalMesh::CPUSkin(const Skeleton& skeleton, const Pose& pose)
{
    const unsigned int numVerts = m_Position.size();
    if (numVerts == 0)
    {
        return;
    }
    
    m_SkinnedPosition.resize(numVerts);
    m_SkinnedNormal.resize(numVerts);

    pose.GetMatrixPalette(m_PosePalette);
    const std::vector<Mat4>& invPosePalette = skeleton.GetInvBindPose();

    for (unsigned int i = 0; i < numVerts; ++i)
    {
        Mat4 skinMatrix = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        for (unsigned int j = 0; j < 4; ++j)
        {
            int boneID = m_BonesID[i][j];
            skinMatrix += m_PosePalette[boneID] * invPosePalette[boneID] * m_BonesWeight[i][j];
        }
        
        m_SkinnedPosition[i] = skinMatrix.TransformPoint(m_Position[i]);
        m_SkinnedNormal[i] = skinMatrix.TransformVector(m_Normal[i]);
    }
    
    m_PositionAttribute->Set(m_SkinnedPosition);
    m_NormalAttribute->Set(m_SkinnedNormal);
    
} // CPUSkin

// ---------------------------------------------------------------------------------------------------------------------

void SkeletalMesh::CPUSkin(const std::vector<Mat4>& animatedPose)
{
    const unsigned int numVerts = m_Position.size();
    if (numVerts == 0)
    {
        return;
    }

    m_SkinnedPosition.resize(numVerts);
    m_SkinnedNormal.resize(numVerts);

    for (unsigned int i = 0; i < numVerts; ++i)
    {
        m_SkinnedPosition[i] = {0, 0, 0};
        m_SkinnedNormal[i] = {0, 0, 0};
        
        for (unsigned int j = 0; j < 4; ++j)
        {
            const Mat4& animatedPoseMatrix = animatedPose[m_BonesID[i][j]];
            const float boneWeight = m_BonesWeight[i][j];
            
            m_SkinnedPosition[i] += animatedPoseMatrix.TransformPoint(m_Position[i]) * boneWeight;
            m_SkinnedNormal[i] += animatedPoseMatrix.TransformVector(m_Normal[i]) * boneWeight;
        }
    }
    
    m_PositionAttribute->Set(m_SkinnedPosition);
    m_NormalAttribute->Set(m_SkinnedNormal);
    
} // CPUSkin

// ---------------------------------------------------------------------------------------------------------------------


