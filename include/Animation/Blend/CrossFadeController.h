#pragma once

#include <vector>

#include "SkeletalMesh/Skeleton.h"

class Skeleton;
class Pose;
template <typename TRACK> struct TCrossFadeTarget;
template <typename TRACK> class TClip;

template <typename TRACK>
class CrossFadeController
{
public:
    CrossFadeController();
    CrossFadeController(const Skeleton& skeleton);

    void SetPlaybackTime(float playbackTime) { m_PlaybackTime = playbackTime; }
    void SetSkeleton(const Skeleton& skeleton);
    
    const Pose& GetCurrentPose() const { return m_Pose; }
    Pose& GetCurrentPose() { return m_Pose; }
    TClip<TRACK>* GetCurrentClip() const { return m_CurrentClip; }

    void Play(TClip<TRACK>* target);
    void FadeTo(TClip<TRACK>* target, float fadeTime);
    void Update(float deltaTime);
    
protected:
    Skeleton m_Skeleton;
    bool m_WasSkeletonSet = false;
    Pose m_Pose;
    std::vector<TCrossFadeTarget<TRACK>> m_Targets;
    TClip<TRACK>* m_CurrentClip = nullptr;
    float m_Time = 0.f;
    float m_PlaybackTime = 1.f;
    
}; // CrossFadeController

