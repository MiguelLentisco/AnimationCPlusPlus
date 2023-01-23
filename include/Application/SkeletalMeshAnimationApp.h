#pragma once

#include "Application.h"
#include "Core/Transform.h"
#include "SkeletalMesh/Skeleton.h"

class SkeletalMesh;
class Shader;
class Texture;
class Clip;

struct AnimationInstance
{
    Skeleton* m_Skeleton = nullptr;
    Pose m_AnimatedPose;
    std::vector<Mat4> m_PosePalette;

    std::vector<Clip>* m_Clips = nullptr;
    int m_CurrentClip = 0;
    float m_PlaybackTime = 0.f;
    
    Transform m_Model;

    void Update(float deltaTime);
    void SwapAnimation(const std::string& clipName);
    void SwapAnimation(unsigned int idx);
    void NextAnimation();
    
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
    
    std::vector<Clip> m_Clips;
    AnimationInstance m_GPUAnimInfo;
    AnimationInstance m_CPUAnimInfo;
    
}; // SkeletalMeshAnimationApp
