#pragma once

#include <vector>

#include "Application.h"
#include "SkeletalMesh/Skeleton.h"

struct Vec3;
struct Quat;
class Texture;
class SkeletalMesh;
class Shader;
template <typename T, unsigned int N> class FastTrack;
template <typename VTRACK, typename QTRACK> class TTransformTrack;
typedef TTransformTrack<FastTrack<Vec3, 3>, FastTrack<Quat, 4>> FastTransformTrack;
template<typename TRACK> class TClip;
typedef TClip<FastTransformTrack> FastClip;

class DualQuatSkinApp : public Application
{
public:
    DualQuatSkinApp();
    
    void Initialize() override;
    void Update(float deltaTime) override;
    void Render(float inAspectRatio) override;
    void Shutdown() override;

protected:
    std::vector<SkeletalMesh> m_Meshes;
    Skeleton m_Skeleton;
    Pose m_CurrentPose;
    std::vector<DualQuaternion> m_DQPosePalette;
    std::vector<DualQuaternion> m_DQInvBindPalette;
    std::vector<Mat4> m_LBPreSkinnedPalette;
    
    std::vector<FastClip> m_Clips;
    unsigned int m_CurrentClip = 0;
    float m_PlaybackTime = 0.f;
    
    Shader* m_LBSShader = nullptr;
    Shader* m_DQShader = nullptr;
    Texture* m_DiffuseTexture = nullptr;
    
}; // DualQuatSkinApp
