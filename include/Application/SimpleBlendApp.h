#pragma once

#include "Application.h"
#include "SkeletalMesh/Skeleton.h"

struct Quat;
struct Vec3;
class SkeletalMesh;
class Shader;
class Texture;
template<typename T, unsigned int N> class FastTrack;
template<typename VTRACK, typename QTRACK> class TTransformTrack;
template<typename TRACK> class TClip;
typedef TClip<TTransformTrack<FastTrack<Vec3, 3>, FastTrack<Quat, 4>>> FastClip;

struct SimpleBlendAnimationInstance
{
    Pose m_Pose;
    
    unsigned int m_CurrentClip = 0;
    float m_Time = 0.f;
    float m_PlaybackSpeed = 1.f;

    template <typename TRACK>
    void Update(const std::vector<TClip<TRACK>>& clips, float deltaTime);
    
}; // AnimationInstance

class SimpleBlendApp : public Application
{
public:
    SimpleBlendApp();
    
    void Initialize() override;
    void Update(float deltaTime) override;
    void Render(float inAspectRatio) override;
    void Shutdown() override;
    
protected:
    Skeleton m_Skeleton;
    std::vector<SkeletalMesh> m_Meshes;
    Pose m_FinalPose;
    std::vector<Mat4> m_PreSkinnedPalette;

    Texture* m_Texture = nullptr;
    Shader* m_Shader = nullptr;
    
    SimpleBlendAnimationInstance m_A1;
    SimpleBlendAnimationInstance m_A2;
    
    std::vector<FastClip> m_Clips;
    float m_PlaybackSpeed = 1.f;
    float m_BlendTime = .0f;
    bool bInvertBlend = false;
    
}; // SimpleBlendApp