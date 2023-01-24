#pragma once

#include <string>

#include "Application.h"
#include "SkeletalMesh/Skeleton.h"

struct Vec3;
struct Quat;
template<typename T, unsigned int N> class Track;
template<typename VTRACK, typename QTRACK> class TTransformTrack;
template<typename TRACK> class TClip;
typedef TClip<TTransformTrack<Track<Vec3, 3>, Track<Quat, 4>>> Clip;
class DebugDrawer;

class SkeletalAnimationApp : public Application
{
public:
    SkeletalAnimationApp();
    
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
    void NextAnimation();
    
}; // SkeletalAnimationApp