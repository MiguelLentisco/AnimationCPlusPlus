﻿#pragma once

#include <string>

#include "Application.h"
#include "Animation/Pose.h"

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

    Pose m_RestPose;
    Pose m_CurrentPose;
    
    DebugDrawer* m_RestPoseVisual = nullptr;
    DebugDrawer* m_CurrentPoseVisual = nullptr;
    
    void SwapAnimation(const std::string& clipName);
    void SwapAnimation(unsigned int idx);
    
}; // AnimationTestApp