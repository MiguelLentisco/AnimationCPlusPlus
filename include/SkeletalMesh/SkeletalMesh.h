#pragma once

#include <map>
#include <vector>

struct TriangleMesh;
class Pose;
class Skeleton;
struct Mat4;
template <typename T> class Attribute;
struct Vec3;
template <typename T> struct TVec4;
typedef TVec4<int> IVec4;
typedef TVec4<float> Vec4;
template <typename T> struct TVec2;
typedef TVec2<float> Vec2;
class IndexBuffer;
typedef std::map<int, int> BoneMap;

class SkeletalMesh
{
public:
    SkeletalMesh();
    SkeletalMesh(const SkeletalMesh& other);
    SkeletalMesh& operator=(const SkeletalMesh& other);
    ~SkeletalMesh();

    const std::vector<Vec3>& GetPosition() const { return m_Position; }
    std::vector<Vec3>& GetPosition() { return m_Position; }
    const std::vector<Vec3>& GetNormal() const { return m_Normal; }
    std::vector<Vec3>& GetNormal() { return m_Normal; }
    const std::vector<Vec2>& GetTexCoord() const { return m_TexCoords; }
    std::vector<Vec2>& GetTexCoord() { return m_TexCoords; }
    const std::vector<Vec4>& GetBonesWeight() const { return m_BonesWeight; }
    std::vector<Vec4>& GetBonesWeight() { return m_BonesWeight; }
    const std::vector<IVec4>& GetBonesID() const { return m_BonesID; }
    std::vector<IVec4>& GetBonesID() { return m_BonesID; }
    const std::vector<unsigned int>& GetIndices() const { return m_Indices; }
    std::vector<unsigned int>& GetIndices() { return m_Indices; }
    
    void UpdateOpenGLBuffers(); // Sync with GPU
    void Bind(int position, int normal, int texCoord, int boneWeight, int boneID);
    void Unbind(int position, int normal, int texCoord, int boneWeight, int boneID);
    
    void Draw();
    void DrawInstanced(unsigned int numInstances);

    void CPUSkin(const Skeleton& skeleton, const Pose& pose);
    void CPUSkin(const std::vector<Mat4>& animatedPose);

    void RearrangeMesh(const BoneMap& boneMap);

    std::vector<TriangleMesh> GetTriangles() const;
    void GetTriangles(std::vector<TriangleMesh>& triangles) const;

protected:
    std::vector<Vec3> m_Position;
    std::vector<Vec3> m_Normal;
    std::vector<Vec2> m_TexCoords;
    std::vector<Vec4> m_BonesWeight;
    std::vector<IVec4> m_BonesID;
    std::vector<unsigned int> m_Indices;
    
    Attribute<Vec3>* m_PositionAttribute = nullptr;
    Attribute<Vec3>* m_NormalAttribute = nullptr;
    Attribute<Vec2>* m_UVAttribute = nullptr;
    Attribute<Vec4>* m_BonesWeightAttribute = nullptr;
    Attribute<IVec4>* m_BonesIDAttribute = nullptr;
    IndexBuffer* m_IndexBuffer = nullptr;

    // CPU skinning
    std::vector<Vec3> m_SkinnedPosition;
    std::vector<Vec3> m_SkinnedNormal;
    std::vector<Mat4> m_PosePalette;
    
}; // SkeletalMesh
