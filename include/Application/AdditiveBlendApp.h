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
typedef TTransformTrack<FastTrack<Vec3, 3>, FastTrack<Quat, 4>> FastTransformTrack;
template<typename TRACK> class TClip;
typedef TClip<TTransformTrack<FastTrack<Vec3, 3>, FastTrack<Quat, 4>>> FastClip;

class AdditiveBlendApp : public Application
{
public:
    AdditiveBlendApp();
    
    void Initialize() override;
    void Update(float deltaTime) override;
    void Render(float inAspectRatio) override;
    void Shutdown() override;
    
protected:
    Skeleton m_Skeleton;
    std::vector<SkeletalMesh> m_Meshes;
    std::vector<Mat4> m_PreSkinnedPalette;

    Pose m_CurrentPose;
    Pose m_AddBasePose;
    Pose m_AddPose;

    Texture* m_Texture = nullptr;
    Shader* m_Shader = nullptr;
    
    std::vector<FastClip> m_Clips;
    unsigned int m_AdditiveClipIdx = 0;
    unsigned int m_BaseClipIdx = 0;
    float m_PlaybackTime = 0.f;

    float m_AdditiveTime = 0.f;
    float m_AdditiveDirection = 1.f;
    
}; // AdditiveBlendApp