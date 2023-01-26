#pragma once

#include <vector>

struct Transform;

class CCDSolver
{
public:
    CCDSolver();
    
    unsigned int GetSize() const;
    unsigned int GetNumSteps() const;
    float GetThreshold() const;

    void Resize(unsigned int newSize);
    void SetNumSteps(unsigned int numSteps);
    void SetThreshold(float threshold);

    const Transform& operator[](unsigned int idx) const;
    Transform& operator[](unsigned int idx);
    
    Transform GetGlobalTransform(unsigned int idx) const;

    bool Solve(const Transform& target);

protected:
    std::vector<Transform> m_IKChain;
    unsigned int m_NumSteps = 15;
    float m_Threshold = .00001f;
    
}; // CCDSolver
