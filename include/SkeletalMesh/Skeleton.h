#pragma once

#include <map>
#include <vector>
#include <string>

#include "Pose.h"

typedef std::map<int, int> BoneMap;

class Skeleton
{
public:
    Skeleton();
    Skeleton(const Pose& rest, const Pose& bind, std::vector<std::string> names);

    const Pose& GetBindPose() const;
    const Pose& GetRestPose() const;
    const std::vector<Mat4>& GetInvBindPose() const;
    const std::vector<std::string>& GetJointNames() const;
    const std::string& GetJointName(unsigned int idx) const;

    void GetInvBindPose(std::vector<DualQuaternion>& invBindPose) const;

    void Set(const Pose& rest, const Pose& bind, const std::vector<std::string>& names);
    BoneMap RearrangeSkeleton();
    
protected:
    Pose m_RestPose;
    Pose m_BindPose;

    std::vector<Mat4> m_InvBindPose;
    std::vector<std::string> m_JointsNames;

    void UpdateInverseBindPose();
    
}; // Skeleton
