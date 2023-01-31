#pragma once

#include <string>

#include "FABRIKSolver.h"
#include "Animation/Track.h"
#include "SkeletalMesh/Pose.h"

class DebugDrawer;

class IKLeg
{
public:
    IKLeg();
    IKLeg(const Skeleton& skeleton, const std::string& hip, const std::string& knee, const std::string& ankle,
        const std::string& toe);
    IKLeg(const IKLeg& other);
    IKLeg& operator=(const IKLeg& other);
    ~IKLeg();

    const Pose& GetPose() const { return m_IKPose; }
    unsigned int GetHipIdx() const { return m_HipIdx; }
    unsigned int GetKneeIdx() const { return m_KneeIdx; }
    unsigned int GetAnkleIdx() const { return m_AnkleIdx; }
    unsigned int GetToeIdx() const { return m_ToeIdx; }
    float GetAnkleOffset() const { return m_AnkleOffset; }

    void SetAnkleOffset(float ankleOffset) { m_AnkleOffset = ankleOffset; }
    void SetPinTrack(const ScalarTrack& pinTrack) { m_PinTrack = pinTrack; }

    void SolveForLeg(const Transform& model, const Pose& pose, const Vec3& ankleTargetPosition);
    float GetPinValue(float alpha) const;
    void Draw(const Mat4& mv, const Vec3& legColor) const;

protected:
    Pose m_IKPose;
    FABRIKSolver m_Solver;
    ScalarTrack m_PinTrack;

    unsigned int m_HipIdx = 0;
    unsigned int m_KneeIdx = 0;
    unsigned int m_AnkleIdx = 0;
    unsigned int m_ToeIdx = 0;

    DebugDrawer* m_LineVisuals = nullptr;
    DebugDrawer* m_PointVisuals = nullptr;
    
    float m_AnkleOffset = 0.f; // The ankle is not flat on the ground
    
}; // DebugDrawer
