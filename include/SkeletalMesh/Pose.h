#pragma once

#include <vector>

struct DualQuaternion;
class Skeleton;
struct Mat4;
struct Transform;

class Pose
{
public:
    Pose(unsigned int numJoints = 0);
    Pose(const Pose& p);
    Pose& operator=(const Pose& p);
    ~Pose();

    unsigned int GetSize() const;
    int GetParent(unsigned int idx) const;
    const Transform& GetLocalTransform(unsigned int idx) const;
    
    void Resize(unsigned int size);
    void SetParent(unsigned int idx, int parent);
    void SetLocalTransform(unsigned int idx, const Transform& transform);

    Transform GetGlobalTransform(unsigned int idx) const;
    Transform operator[](unsigned int idx) const;
    DualQuaternion GetGlobalDualQuaternion(unsigned int idx) const;

    void GetMatrixPalette(std::vector<Mat4>& out) const;
    void GetMatrixPreSkinnedPalette(std::vector<Mat4>& out, const Skeleton& skeleton) const;
    void GetDualQuaternionPalette(std::vector<DualQuaternion>& out) const;

    bool operator==(const Pose& other) const;
    bool operator !=(const Pose& other) const;

    bool IsInHierarchy(unsigned int root, unsigned int search) const;
    static void Blend(Pose& blendedPose, const Pose& start, const Pose& end, float alpha, int rootBone);
    static void Add(Pose& addedPose, const Pose& inputPose, const Pose& addPose, const Pose& basePose, int blendRoot);
    
protected:
    std::vector<Transform> m_Joints;
    std::vector<int> m_Parents;
    
}; // Pose
