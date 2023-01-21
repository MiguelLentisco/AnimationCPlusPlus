#include "SkeletalMesh/Skeleton.h"

#include "Core/Mat4.h"
#include "Core/Transform.h"

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

void Skeleton::Set(const Pose& rest, const Pose& bind, const std::vector<std::string>& names)
{
    m_RestPose = rest;
    m_BindPose = bind;
    m_JointsNames = names;
    UpdateInverseBindPose();
    
} // Set

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
