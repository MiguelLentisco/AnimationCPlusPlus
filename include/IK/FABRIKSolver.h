#pragma once

#include <vector>

struct Vec3;
struct Transform;

class FABRIKSolver
{
public:
    FABRIKSolver();

    unsigned int GetSize() const;
    unsigned int GetNumSteps() const;
    float GetThreshold() const;
    const Transform& GetLocalTransform(unsigned int idx) const;
    Transform& GetLocalTransform(unsigned int idx);

    const Transform& operator[](unsigned int idx) const;
    Transform& operator[](unsigned int idx);

    void Resize(unsigned int newSize);
    void SetNumSteps(unsigned int numSteps);
    void SetThreshold(float threshold);
    void SetLocalTransform(unsigned int idx, const Transform& t);

    Transform GetGlobalTransform(unsigned int idx) const;

    bool Solve(const Transform& target);

protected:
    std::vector<Transform> m_IKChain;
    unsigned int m_NumSteps = 15;
    float m_Threshold = .00001f;

    std::vector<Vec3> m_WorldChain;
    std::vector<float> m_Lengths;

    void IKChainToWorld();
    void IterateForward(const Vec3& base);
    void IterateBackwards(const Vec3& goal);
    void WorldToIKChain();
    
}; // FABRIKSolver
