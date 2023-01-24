#pragma once

#include "Application.h"
#include "Core/Transform.h"
#include "SkeletalMesh/Skeleton.h"

class SkeletalMesh;
class Shader;
class Texture;
template<typename T, unsigned int N> class FastTrack;
template<typename VTRACK, typename QTRACK> class TTransformTrack;
template<typename TRACK> class TClip;
typedef TClip<TTransformTrack<FastTrack<Vec3, 3>, FastTrack<Quat, 4>>> FastClip;

struct AnimationInstance
{
    Skeleton* m_Skeleton = nullptr;
    Pose m_AnimatedPose;
    std::vector<Mat4> m_PosePalette;

    std::vector<FastClip>* m_Clips = nullptr;
    int m_CurrentClip = 0;
    float m_PlaybackTime = .0f;
    float m_PlaybackSpeed = 1.f;
    bool bLoop = true;
    
    Transform m_Model;

    void Update(float deltaTime);
    void SwapAnimation(const std::string& clipName);
    void SwapAnimation(unsigned int idx);
    void NextAnimation();

private:
    void UpdateAnimation(float deltaTime);
    
}; // AnimationInstance

class SkeletalMeshAnimationApp : public Application
{
public:
    SkeletalMeshAnimationApp();
    
    void Initialize() override;
    void Update(float deltaTime) override;
    void Render(float inAspectRatio) override;
    void Shutdown() override;
    
protected:
    Skeleton m_Skeleton;
    std::vector<SkeletalMesh> m_CPUMeshes;
    std::vector<SkeletalMesh> m_GPUMeshes;

    Texture* m_DiffuseTexture = nullptr;
    Shader* m_StaticShader = nullptr;
    Shader* m_SkinnedShader = nullptr;
    
    std::vector<FastClip> m_Clips;
    AnimationInstance m_GPUAnimInfo;
    AnimationInstance m_CPUAnimInfo;
    
}; // SkeletalMeshAnimationApp
