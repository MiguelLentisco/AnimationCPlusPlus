#include "IK/FABRIKSolver.h"

#include "Core/Transform.h"

// ---------------------------------------------------------------------------------------------------------------------

FABRIKSolver::FABRIKSolver()
{
    
} // FABRIKSolver

// ---------------------------------------------------------------------------------------------------------------------

unsigned FABRIKSolver::GetSize() const
{
    return m_IKChain.size();
    
} // GetSize

// ---------------------------------------------------------------------------------------------------------------------

unsigned FABRIKSolver::GetNumSteps() const
{
    return m_NumSteps;
    
} // GetNumSteps

// ---------------------------------------------------------------------------------------------------------------------

float FABRIKSolver::GetThreshold() const
{
    return m_Threshold;
    
} // GetThreshold

// ---------------------------------------------------------------------------------------------------------------------

const Transform& FABRIKSolver::GetLocalTransform(unsigned idx) const
{
    return m_IKChain[idx];
    
} // GetLocalTransform

// ---------------------------------------------------------------------------------------------------------------------

Transform& FABRIKSolver::GetLocalTransform(unsigned idx)
{
    return m_IKChain[idx];
    
} // GetLocalTransform

// ---------------------------------------------------------------------------------------------------------------------

const Transform& FABRIKSolver::operator[](unsigned idx) const
{
    return m_IKChain[idx];
    
} // operator[]

// ---------------------------------------------------------------------------------------------------------------------

Transform& FABRIKSolver::operator[](unsigned idx)
{
    return m_IKChain[idx];
    
} // operator[]

// ---------------------------------------------------------------------------------------------------------------------

void FABRIKSolver::Resize(unsigned newSize)
{
    m_IKChain.resize(newSize);
    m_WorldChain.resize(newSize);
    m_Lengths.resize(newSize);
    
} // Resize

// ---------------------------------------------------------------------------------------------------------------------

void FABRIKSolver::SetNumSteps(unsigned numSteps)
{
    m_NumSteps = numSteps;
    
} // SetNumSteps

// ---------------------------------------------------------------------------------------------------------------------

void FABRIKSolver::SetThreshold(float threshold)
{
    m_Threshold = threshold;
    
} // SetThreshold

// ---------------------------------------------------------------------------------------------------------------------

void FABRIKSolver::SetLocalTransform(unsigned idx, const Transform& t)
{
    m_IKChain[idx] = t;
    
} // SetLocalTransform

// ---------------------------------------------------------------------------------------------------------------------

Transform FABRIKSolver::GetGlobalTransform(unsigned idx) const
{
    Transform worldTransform = m_IKChain[idx];
    for (int i = static_cast<int>(idx - 1); i >= 0; --i)
    {
        worldTransform = m_IKChain[i].Combine(worldTransform);
    }
    return worldTransform;
    
} // GetGlobalTransform

// ---------------------------------------------------------------------------------------------------------------------

bool FABRIKSolver::Solve(const Transform& target)
{
    if (m_IKChain.empty())
    {
        return false;
    }

    const unsigned int size = GetSize();
    const unsigned int lastIdx = size - 1;
    const float thresholdSq = m_Threshold * m_Threshold;
    const Vec3& goal = target.position;

    auto HasReachedGoal = [&](const Vec3& effectorPos) -> bool
    {
        return (goal - effectorPos).LenSq() < thresholdSq;
    };

    if (HasReachedGoal(GetGlobalTransform(lastIdx).position))
    {
        return true;
    }

    IKChainToWorld();
    const Vec3 base = m_WorldChain[0];

    for (unsigned int i = 0; i < m_NumSteps; ++i)
    {
        IterateBackwards(goal);
        IterateForward(base);

        // Check constrains
        WorldToIKChain();
        // Constrains
        IKChainToWorld();

        if (HasReachedGoal(m_WorldChain[lastIdx]))
        {
            WorldToIKChain();
            return true;
        }
    }

    WorldToIKChain();
    return false;
    
} // Solve

// ---------------------------------------------------------------------------------------------------------------------

void FABRIKSolver::IKChainToWorld()
{
    if (m_IKChain.empty())
    {
        return;
    }

    m_WorldChain[0] = GetGlobalTransform(0).position;
    m_Lengths[0] = 0.f;
    
    const unsigned int size = GetSize();
    for (unsigned int i = 1; i < size; ++i)
    {
        m_WorldChain[i] = GetGlobalTransform(i).position;
        m_Lengths[i] = (m_WorldChain[i] - m_WorldChain[i - 1]).Len();
    }
    
} // IKChainToWorld

// ---------------------------------------------------------------------------------------------------------------------

void FABRIKSolver::IterateForward(const Vec3& base)
{
    if (m_IKChain.empty())
    {
        return;
    }

    const unsigned int size = GetSize();
    m_WorldChain[0] = base;

    for (unsigned int i = 1; i < size; ++i)
    {
        const Vec3 direction = (m_WorldChain[i] - m_WorldChain[i - 1]).Normalized();
        const Vec3 offset = direction * m_Lengths[i];
        m_WorldChain[i] = m_WorldChain[i - 1] + offset;
    }
    
} // IterateForward

// ---------------------------------------------------------------------------------------------------------------------

void FABRIKSolver::IterateBackwards(const Vec3& goal)
{
    if (m_IKChain.empty())
    {
        return;
    }

    const int size = static_cast<int>(GetSize());
    m_WorldChain[size - 1] = goal;

    for (int i = size - 2; i >= 0; --i)
    {
        const Vec3 direction = (m_WorldChain[i] - m_WorldChain[i + 1]).Normalized();
        const Vec3 offset = direction * m_Lengths[i + 1];
        m_WorldChain[i] = m_WorldChain[i + 1] + offset;
    }
    
} // IterateBackwards

// ---------------------------------------------------------------------------------------------------------------------

void FABRIKSolver::WorldToIKChain()
{
    if (m_IKChain.empty())
    {
        return;
    }

    const unsigned int size = GetSize();
    for (unsigned int i = 0; i < size - 1; ++i)
    {
        const Transform currentTransform = GetGlobalTransform(i);
        const Transform nextTransform = GetGlobalTransform(i + 1);

        // Apply rotation from current to desired rotation
        const Quat currentRotInverse = currentTransform.rotation.Inverse();
        const Vec3 toNext = currentRotInverse * (nextTransform.position - currentTransform.position);
        const Vec3 toDesired = currentRotInverse * (m_WorldChain[i + 1] - currentTransform.position);

        const Quat moveRot = Quat::FromTo(toNext, toDesired);
        m_IKChain[i].rotation = moveRot * m_IKChain[i].rotation;
    }
    
} // WorldToIKChain

// ---------------------------------------------------------------------------------------------------------------------