#include "IK/CCDSolver.h"

#include "Core/Transform.h"

// ---------------------------------------------------------------------------------------------------------------------

CCDSolver::CCDSolver()
{
    
} // CCDSolver

// ---------------------------------------------------------------------------------------------------------------------

unsigned CCDSolver::GetSize() const
{
    return m_IKChain.size();
    
} // GetSize

// ---------------------------------------------------------------------------------------------------------------------

unsigned CCDSolver::GetNumSteps() const
{
    return m_NumSteps;
    
} // GetNumSteps

// ---------------------------------------------------------------------------------------------------------------------

float CCDSolver::GetThreshold() const
{
    return m_Threshold;
    
} // GetThreshold

// ---------------------------------------------------------------------------------------------------------------------

void CCDSolver::Resize(unsigned newSize)
{
    m_IKChain.resize(newSize);
    
} // Resize

// ---------------------------------------------------------------------------------------------------------------------

void CCDSolver::SetNumSteps(unsigned numSteps)
{
    m_NumSteps = numSteps;
    
} // SetNumSteps

// ---------------------------------------------------------------------------------------------------------------------

void CCDSolver::SetThreshold(float threshold)
{
    m_Threshold = threshold;
    
} // SetThreshold

// ---------------------------------------------------------------------------------------------------------------------

const Transform& CCDSolver::operator[](unsigned idx) const
{
    return m_IKChain[idx];
    
} // operator[]

// ---------------------------------------------------------------------------------------------------------------------

Transform& CCDSolver::operator[](unsigned idx)
{
    return m_IKChain[idx];
    
} // operator[]

// ---------------------------------------------------------------------------------------------------------------------

Transform CCDSolver::GetGlobalTransform(unsigned idx) const
{
    Transform worldTransform = m_IKChain[idx];
    for (int i = static_cast<int>(idx - 1); i >= 0; --i)
    {
        worldTransform = m_IKChain[i].Combine(worldTransform);
    }
    return worldTransform;
    
} // Transform

// ---------------------------------------------------------------------------------------------------------------------

bool CCDSolver::Solve(const Transform& target)
{
    if (m_IKChain.empty())
    {
        return false;
    }

    const unsigned int size = GetSize();
    const unsigned int lastIdx = size - 1;
    const float thresholdSq = m_Threshold * m_Threshold;
    const Vec3& goal = target.position;
    
    auto HasReachedGoal = [&]() -> bool
    {
        const Vec3 effectorPos = GetGlobalTransform(lastIdx).position;
        return (goal - effectorPos).LenSq() < thresholdSq;
    };

    if (HasReachedGoal())
    {
        return true;
    }
        
    for (unsigned int i = 0; i < m_NumSteps; ++i)
    {
        for (int j = static_cast<int>(size - 2); j >= 0; --j)
        {
           const Vec3 effectorPos = GetGlobalTransform(lastIdx).position;
            
            const Transform jointWorldTransform = GetGlobalTransform(j);
            const Vec3 toEffector = effectorPos - jointWorldTransform.position;
            const Vec3 toGoal = goal - jointWorldTransform.position;

            static constexpr float EPSILON = .00001f;
            Quat effectorToGoal = toGoal.LenSq() > EPSILON ? Quat::FromTo(toEffector, toGoal) : Quat{};

            // Rotate joint in world space then back to joint space
            const Quat worldRotated = jointWorldTransform.rotation * effectorToGoal;
            const Quat localRotated = worldRotated * jointWorldTransform.rotation.Inverse();
            m_IKChain[j].rotation = localRotated * m_IKChain[j].rotation;

            // We don't need to rotate all joints
            if (HasReachedGoal())
            {
                return true;
            }
        }
    }

    // Check from last iteration
    return false;
    
} // Solve

// ---------------------------------------------------------------------------------------------------------------------
