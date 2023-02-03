#pragma once

#include <vector>

#include "Application/Application.h"
#include "SkeletalMesh/Skeleton.h"

struct Vec3;
struct Quat;
class Crowd;
class AnimTexture;
class SkeletalMesh;
class Shader;
class Texture;

template <typename T, unsigned int N> class Track;
template <typename T, unsigned int N> class FastTrack;
template <typename VTRACK, typename QTRACK> class TTransformTrack;
typedef TTransformTrack<Track<Vec3, 3>, Track<Quat, 4>> TransformTrack;
typedef TTransformTrack<FastTrack<Vec3, 3>, FastTrack<Quat, 4>> FastTransformTrack;
template<typename TRACK> class TClip;
typedef TClip<TransformTrack> Clip;
typedef TClip<FastTransformTrack> FastClip;

class CrowdApp : public Application
{
public:
    CrowdApp();
    
    void Initialize() override;
    void Update(float deltaTime) override;
    void Render(float inAspectRatio) override;
    void Shutdown() override;
    
protected:
    std::vector<SkeletalMesh> m_Meshes;
    Skeleton m_Skeleton;
    Texture* m_DiffuseTexture = nullptr;
    
    std::vector<AnimTexture> m_AnimTextures;
    std::vector<Crowd> m_Crowds;
    std::vector<FastClip> m_Clips;
   
    Shader* m_CrowdShader = nullptr;

    void SetCrowdSize(unsigned int size);
    
}; // Application
