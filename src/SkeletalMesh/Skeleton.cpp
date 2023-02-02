#include "SkeletalMesh/Skeleton.h"

#include <list>

#include "Core/DualQuaternion.h"
#include "Core/Mat4.h"
#include "Core/Transform.h"
#include "SkeletalMesh/SkeletalMesh.h"

// ---------------------------------------------------------------------------------------------------------------------

Skeleton::Skeleton()
{
    
} // Skeleton

// ---------------------------------------------------------------------------------------------------------------------

Skeleton::Skeleton(const Pose& rest, const Pose& bind, std::vector<std::string> names) : m_RestPose(rest),
    m_BindPose(bind), m_JointsNames(std::move(names))
{
    UpdateInverseBindPose();
    
} // Skeleton

// ---------------------------------------------------------------------------------------------------------------------

const Pose& Skeleton::GetBindPose() const
{
    return m_BindPose;
    
} // GetBindPose

// ---------------------------------------------------------------------------------------------------------------------

const Pose& Skeleton::GetRestPose() const
{
    return m_RestPose;
    
} // GetRestPose

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<Mat4>& Skeleton::GetInvBindPose() const
{
    return m_InvBindPose;
    
} // GetInvBindPose

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<std::string>& Skeleton::GetJointNames() const
{
    return m_JointsNames;
    
} // GetJointNames

// ---------------------------------------------------------------------------------------------------------------------

const std::string& Skeleton::GetJointName(unsigned idx) const
{
    return m_JointsNames[idx];
    
} // GetJointName

// ---------------------------------------------------------------------------------------------------------------------

void Skeleton::GetInvBindPose(std::vector<DualQuaternion>& invBindPose) const
{
    const unsigned int size = m_BindPose.GetSize();
    invBindPose.resize(size);

    for (unsigned int i = 0; i < size; ++i)
    {
        invBindPose[i] = m_BindPose.GetGlobalDualQuaternion(i).Conjugated();
    }
    
} // GetInvBindPose

// ---------------------------------------------------------------------------------------------------------------------

void Skeleton::Set(const Pose& rest, const Pose& bind, const std::vector<std::string>& names)
{
    m_RestPose = rest;
    m_BindPose = bind;
    m_JointsNames = names;
    UpdateInverseBindPose();
    
} // Set

// ---------------------------------------------------------------------------------------------------------------------

BoneMap Skeleton::RearrangeSkeleton()
{
    const unsigned int size = m_RestPose.GetSize();
    if (size == 0)
    {
        return {};
    }

    std::vector<std::vector<unsigned int>> hierarchy(size);
    std::list<unsigned int> process;

    for (unsigned int i = 0; i < size; ++i)
    {
        const int parent = m_RestPose.GetParent(i);
        if (parent > 0)
        {
            hierarchy[parent].push_back(i);
        }
        else
        {
            process.push_back(i);
        }
    }

    BoneMap oldToNewID;
    BoneMap newToOldID;
    
    for (unsigned int newID = 0; newID < size; ++newID, process.pop_front())
    {
        const int oldID = static_cast<int>(process.front());
        const int iNewID = static_cast<int>(newID);
        newToOldID[iNewID] = oldID;
        oldToNewID[oldID] = iNewID;
        
        for (const unsigned int childrenID : hierarchy[oldID])
        {
            process.push_back(childrenID);
        }
    }

    oldToNewID[-1] = -1;
    newToOldID[-1] = -1;

    Pose newRestPose(size);
    Pose newBindPose(size);
    std::vector<std::string> newNames(size);

    for (unsigned int i = 0; i < size; ++i)
    {
        const int oldID = newToOldID[static_cast<int>(i)];
        newRestPose.SetLocalTransform(i, m_RestPose.GetLocalTransform(oldID));
        newBindPose.SetLocalTransform(i, m_BindPose.GetLocalTransform(oldID));
        newNames[i] = GetJointName(oldID);

        const int newParentID = oldToNewID[m_BindPose.GetParent(oldID)];
        newRestPose.SetParent(i, newParentID);
        newBindPose.SetParent(i, newParentID);
    }

    Set(newRestPose, newBindPose, newNames);
    return oldToNewID;
    
} // RearrangeSkeleton

// ---------------------------------------------------------------------------------------------------------------------

void Skeleton::UpdateInverseBindPose()
{
    const unsigned int size = m_JointsNames.size();
    m_InvBindPose.resize(size);

    for (unsigned int i = 0; i < size; ++i)
    {
        const Transform world = m_BindPose.GetGlobalTransform(i);
        m_InvBindPose[i] = world.ToMat4().Inverse();
    }
    
} // UpdateInverseBindPose

// ---------------------------------------------------------------------------------------------------------------------
