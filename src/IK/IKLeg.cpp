#include "IK/IKLeg.h"

#include "Core/Transform.h"
#include "Render/DebugDrawer.h"
#include "SkeletalMesh/Skeleton.h"

// ---------------------------------------------------------------------------------------------------------------------

IKLeg::IKLeg() : m_LineVisuals(new DebugDrawer()), m_PointVisuals(new DebugDrawer())
{
    m_Solver.Resize(3);
    m_PointVisuals->Resize(3);
    m_LineVisuals->Resize(4);
    
} // IKLeg

// ---------------------------------------------------------------------------------------------------------------------

IKLeg::IKLeg(const Skeleton& skeleton, const std::string& hip, const std::string& knee, const std::string& ankle,
    const std::string& toe) : IKLeg()
{
    const unsigned int numBones = skeleton.GetRestPose().GetSize();
    for (unsigned int i = 0; i < numBones; ++i)
    {
        const std::string& jointName = skeleton.GetJointName(i);
        if (jointName == hip)
        {
            m_HipIdx = i;
        }
        else if (jointName == knee)
        {
            m_KneeIdx = i;
        }
        else if (jointName == ankle)
        {
            m_AnkleIdx = i;
        }
        else if (jointName == toe)
        {
            m_ToeIdx = i;
        }
    }
    
} // IKLeg

// ---------------------------------------------------------------------------------------------------------------------

IKLeg::IKLeg(const IKLeg& other) : IKLeg()
{
    *this = other;
    
} // IKLeg

// ---------------------------------------------------------------------------------------------------------------------

IKLeg& IKLeg::operator=(const IKLeg& other)
{
    if (this == &other)
    {
        return *this;
    }

    m_Solver = other.m_Solver;
    m_AnkleOffset = other.m_AnkleOffset;
    m_HipIdx = other.m_HipIdx;
    m_KneeIdx = other.m_KneeIdx;
    m_AnkleIdx = other.m_AnkleIdx;
    m_ToeIdx = other.m_ToeIdx;

    return *this;
    
} // operator=

// ---------------------------------------------------------------------------------------------------------------------

IKLeg::~IKLeg()
{
    delete m_LineVisuals;
    delete m_PointVisuals;
    
} // ~IKLeg

// ---------------------------------------------------------------------------------------------------------------------

void IKLeg::SolveForLeg(const Transform& model, const Pose& pose, const Vec3& ankleTargetPosition)
{
    // Set solver with leg positions
    m_Solver.SetLocalTransform(0, model.Combine(pose.GetGlobalTransform(m_HipIdx)));
    m_Solver.SetLocalTransform(1, pose.GetLocalTransform(m_KneeIdx));
    m_Solver.SetLocalTransform(2, pose.GetLocalTransform(m_AnkleIdx));
    m_IKPose = pose;

    const Transform target(ankleTargetPosition + Vec3{0, m_AnkleOffset, 0});
    m_Solver.Solve(target);

    // Set pose with the new leg positions
    const Transform rootWorld = model.Combine(pose.GetGlobalTransform(pose.GetParent(m_HipIdx)));
    m_IKPose.SetLocalTransform(m_HipIdx, rootWorld.Inverse().Combine(m_Solver.GetLocalTransform(0)));
    m_IKPose.SetLocalTransform(m_KneeIdx, m_Solver.GetLocalTransform(1));
    m_IKPose.SetLocalTransform(m_AnkleIdx, m_Solver.GetLocalTransform(2));

    m_LineVisuals->LinesFromIKSolver(m_Solver);
    m_PointVisuals->PointsFromIKSolver(m_Solver);
    
} // SolveForLeg

// ---------------------------------------------------------------------------------------------------------------------

float IKLeg::GetPinValue(float alpha) const
{
    return m_PinTrack.Sample(alpha, true);
    
} // GetPinValue

// ---------------------------------------------------------------------------------------------------------------------

void IKLeg::Draw(const Mat4& mv, const Vec3& legColor) const
{
    m_LineVisuals->UpdateOpenGLBuffers();
    m_PointVisuals->UpdateOpenGLBuffers();
    m_LineVisuals->Draw(DebugDrawMode::Lines, legColor, mv);
    m_PointVisuals->Draw(DebugDrawMode::Points, legColor, mv);
    
} // Draw

// ---------------------------------------------------------------------------------------------------------------------
