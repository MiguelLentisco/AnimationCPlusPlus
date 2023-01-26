#pragma once

#include "SkeletalMesh/Pose.h"

template <typename TRACK> class TClip;

template <typename TRACK>
struct TCrossFadeTarget
{
    Pose m_Pose;
    TClip<TRACK>* m_Clip = nullptr;
    float m_Time = 0.f;
    float m_Duration = 0.f;
    float m_Elapsed = 0.f;

    TCrossFadeTarget() {}
    TCrossFadeTarget(TClip<TRACK>* clip, const Pose& pose, float duration);
    
}; // CrossFadeTarget
