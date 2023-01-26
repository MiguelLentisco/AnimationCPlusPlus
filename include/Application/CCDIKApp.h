#pragma once

#include "Application.h"
#include "Animation/Track.h"
#include "Animation/TransformTrack.h"
#include "Core/Transform.h"
#include "IK/CCDSolver.h"

class DebugDrawer;

class CCDIKApp : public Application
{
public:
    CCDIKApp() {}

    void Initialize() override;
    void Update(float inDeltaTime) override;
    void Render(float inAspectRatio) override;
    void Shutdown() override;
    
protected:
    Transform m_Target;
    CCDSolver m_Solver;
    
    float m_PlayTime = 0.f;
    TransformTrack m_TargetPath;

    DebugDrawer* m_SolverLines = nullptr;
    DebugDrawer* m_SolverPoints = nullptr;
    DebugDrawer* mTargetVisual[3] = {nullptr, nullptr, nullptr};

    // Cam settings
    float m_CamPitch = 45.f;
    float m_CamYaw = 60.f;
    float m_CamDist = 7.f;

    static void SetFrame(VectorTrack& track, int index, float time, const Vec3& value);
    
}; // CCDIKApp
