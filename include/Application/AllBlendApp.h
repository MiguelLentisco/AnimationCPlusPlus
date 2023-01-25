#pragma once

#include "Application.h"
#include "Animation/Blend/CrossFadeController.h"
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

class AllBlendApp : public Application
{
public:
    AllBlendApp();
    
    void Initialize() override;
    void Update(float deltaTime) override;
    void Render(float inAspectRatio) override;
    void Shutdown() override;
    
protected:
    Skeleton m_Skeleton;
    std::vector<SkeletalMesh> m_Meshes;
    std::vector<Mat4> m_PreSkinnedPalette;

    Texture* m_Texture = nullptr;
    Shader* m_Shader = nullptr;

    CrossFadeController<FastTransformTrack> m_FadeController;
    std::vector<FastClip> m_Clips;
    unsigned int m_CurrentClip = 0;
    float m_FadeOutCooldown = 2.5f;
    float m_FadeOutTimer = m_FadeOutCooldown;
    float m_FadeOutTime = .4f;
    
}; // SimpleBlendApp