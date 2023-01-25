#include "SkeletalMesh/Pose.h"

#include "Core/Mat4.h"
#include "Core/Transform.h"

// ---------------------------------------------------------------------------------------------------------------------

Pose::Pose(unsigned numJoints)
{
    Resize(numJoints);
    
} // Pose

// ---------------------------------------------------------------------------------------------------------------------

Pose::Pose(const Pose& p)
{
    *this = p;
    
} // Pose

// ---------------------------------------------------------------------------------------------------------------------

Pose& Pose::operator=(const Pose& p)
{
    if (&p == this)
    {
        return *this;
    }

    const unsigned int parentsSize = p.m_Parents.size();
    if (m_Parents.size() != parentsSize)
    {
        m_Parents.resize(parentsSize);
    }

    const unsigned int jointsSize = p.m_Parents.size();
    if (m_Joints.size() != jointsSize)
    {
        m_Joints.resize(jointsSize);
    }

    if (parentsSize > 0)
    {
        memcpy(m_Parents.data(), p.m_Parents.data(), sizeof(int) * parentsSize);
    }

    if (jointsSize > 0)
    {
        memcpy(m_Joints.data(), p.m_Joints.data(), sizeof(Transform) * jointsSize);
    }

    return *this;
    
} // operator=

// ---------------------------------------------------------------------------------------------------------------------

Pose::~Pose()
{
}

// ---------------------------------------------------------------------------------------------------------------------

unsigned Pose::GetSize() const
{
    return m_Joints.size();
    
} // GetSize

// ---------------------------------------------------------------------------------------------------------------------

int Pose::GetParent(unsigned idx) const
{
    return m_Parents[idx];
     
} // GetParent

// ---------------------------------------------------------------------------------------------------------------------

const Transform& Pose::GetLocalTransform(unsigned idx) const
{
    return m_Joints[idx];
    
} // GetLocalTransform

// ---------------------------------------------------------------------------------------------------------------------

void Pose::Resize(unsigned size)
{
    m_Parents.resize(size);
    m_Joints.resize(size);
    
} // Resize

// ---------------------------------------------------------------------------------------------------------------------

void Pose::SetParent(unsigned idx, int parent)
{
    m_Parents[idx] = parent;
    
} // SetParent

// ---------------------------------------------------------------------------------------------------------------------

void Pose::SetLocalTransform(unsigned idx, const Transform& transform)
{
    m_Joints[idx] = transform;
    
} // SetLocalTransform

// ---------------------------------------------------------------------------------------------------------------------

Transform Pose::GetGlobalTransform(unsigned idx) const
{
    Transform result = m_Joints[idx];
    for (int p = m_Parents[idx]; p >= 0; p = m_Parents[p])
    {
        result = m_Joints[p].Combine(result);
    }

    return result;
    
} // GetGlobalTransform

// ---------------------------------------------------------------------------------------------------------------------

Transform Pose::operator[](unsigned idx) const
{
    return GetGlobalTransform(idx);
    
} // operator[]

// ---------------------------------------------------------------------------------------------------------------------

void Pose::GetMatrixPalette(std::vector<Mat4>& out) const
{
    const int size = static_cast<int>(GetSize());
    out.resize(size);

    int i = 0;

    // Optimized method only for ordered bones
    for (; i < size; ++i)
    {
        const int parent = m_Parents[i];
        if (parent > i)
        {
            break;
        }

        Mat4 global = m_Joints[i].ToMat4();
        if (parent >= 0)
        {
            global = out[parent] * global;
        }
        out[i] = global;
    }

    // If not ordered, use unoptimized method
    for (; i < size; ++i)
    {
        out[i] = GetGlobalTransform(i).ToMat4();
    }
    
} // GetMatrixPalette

// ---------------------------------------------------------------------------------------------------------------------

bool Pose::operator==(const Pose& other) const
{
    const unsigned int jointsSize = m_Joints.size();
    if (jointsSize != other.m_Joints.size() || m_Parents.size() != other.m_Parents.size())
    {
        return false;
    }

    for (unsigned int i = 0; i < jointsSize; ++i)
    {
        if (m_Parents[i] != other.m_Parents[i] || m_Joints[i] != other.m_Joints[i])
        {
            return false;
        }
    }

    return true;
    
} // operator==

// ---------------------------------------------------------------------------------------------------------------------

bool Pose::operator!=(const Pose& other) const
{
    return !(*this == other);
    
} // operator!=

// ---------------------------------------------------------------------------------------------------------------------

bool Pose::IsInHierarchy(unsigned root, unsigned search) const
{
    if (search == root)
    {
        return true;
    }

    const int iRoot = static_cast<int>(root);
    for (int boneID = GetParent(search); boneID >= 0; boneID = GetParent(boneID))
    {
        if (boneID == iRoot)
        {
            return true;
        }
    }

    return false;
    
} // IsInHierarchy

// ---------------------------------------------------------------------------------------------------------------------

void Pose::Blend(Pose& blendedPose, const Pose& start, const Pose& end, float alpha, int rootBone)
{
    const unsigned numBones = blendedPose.GetSize();
    for (unsigned int i = 0; i < numBones; ++i)
    {
        if (rootBone >= 0 && !blendedPose.IsInHierarchy(rootBone, i))
        {
            continue;
        }

        blendedPose.SetLocalTransform(i, Transform::Mix(start.GetLocalTransform(i), end.GetLocalTransform(i), alpha));
    }
    
} // Blend

// ---------------------------------------------------------------------------------------------------------------------

void Pose::Add(Pose& addedPose, const Pose& inputPose, const Pose& addPose, const Pose& basePose, int blendRoot)
{
    const unsigned int numBones = addPose.GetSize();
    for (unsigned int i = 0; i < numBones; ++i)
    {
        const Transform& inputTransform = inputPose.GetLocalTransform(i);
        const Transform& addTransform = addPose.GetLocalTransform(i);
        const Transform& addBasePose = basePose.GetLocalTransform(i);

        if (blendRoot >= 0 && !addPose.IsInHierarchy(blendRoot, i))
        {
            continue;
        }

        // addedPose = inputPose + (addPose - basePose)
        Transform result =
        {
            inputTransform.position + (addTransform.position - addBasePose.position),
            (inputTransform.rotation * (addBasePose.rotation.Inverse() * addTransform.rotation)).Normalized(),
            inputTransform.scale + (addTransform.scale - addBasePose.scale)
        };

        addedPose.SetLocalTransform(i, result);
        
    }
    
} // Add

// ---------------------------------------------------------------------------------------------------------------------
