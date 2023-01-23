#pragma once

#include <string>

#include "Application.h"
#include "SkeletalMesh/Pose.h"
#include "SkeletalMesh/Skeleton.h"

class Clip;
class DebugDrawer;

class AnimationTestApp : public Application
{
public:
    AnimationTestApp();
    
    void Initialize() override;
    void Update(float deltaTime) override;
    void Render(float inAspectRatio) override;
    void Shutdown() override;
    
protected:
    std::vector<Clip> m_Clips;
    int m_CurrentClip = 0;
    float m_PlaybackTime = 0.f;

    Skeleton m_Skeleton;
    Pose m_CurrentPose;
    
    DebugDrawer* m_RestPoseVisual = nullptr;
    DebugDrawer* m_BindPoseVisual = nullptr;
    DebugDrawer* m_CurrentPoseVisual = nullptr;

    bool bShowBasePoses = false;
    
    void SwapAnimation(const std::string& clipName);
    void SwapAnimation(unsigned int idx);
    
}; // AnimationTestApp