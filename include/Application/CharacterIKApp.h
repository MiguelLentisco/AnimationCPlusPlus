#pragma once

#include "Application.h"
#include "Animation/Clip.h"
#include "Animation/TransformTrack.h"
#include "Animation/Track.h"
#include "Core/Transform.h"
#include "SkeletalMesh/Skeleton.h"

class IKLeg;
struct TriangleMesh;
class SkeletalMesh;
class Texture;
class Shader;
class DebugDrawer;

class CharacterIKApp : public Application
{
public:
    CharacterIKApp();

    void Initialize() override;
    void Update(float inDeltaTime) override;
    void Render(float inAspectRatio) override;
    void Shutdown() override;
    
protected:
    // Character
    std::vector<SkeletalMesh> m_CharacterMeshes;
    Texture* m_CharacterTexture = nullptr;
    Transform m_Model;
    float m_LastHeight = 0.f;
    float m_SinkIntoGround = .15f;
    bool bShowCharacter = true;

    // Character's skeletal mesh
    Skeleton m_Skeleton;
    Pose m_CurrentPose;
    std::vector<Mat4> m_PreSkinnedPalette;

    // IK
    IKLeg* m_LeftLeg = nullptr;
	IKLeg* m_RightLeg = nullptr;
    float m_AnkleOffset = 0.2f;
    float m_ToeLength = 0.3f;

    // Animation
    std::vector<FastClip> m_Clips;
    unsigned int m_CurrentClipIdx = 0;
    float m_PlaybackTime = 0.f;
    float m_AnimationSpeed = 1.f;

    // Movement track
    VectorTrack m_MotionTrack;
    float m_WalkingTime = 0.f;
    float m_WalkingSpeed = 0.3f;
    float m_WalkingTotalTime = 6.f;

    // Environment
    std::vector<SkeletalMesh> m_EnvironmentMeshes; // Should be StaticMesh
    Texture* m_EnvironmentTexture = nullptr;
    std::vector<TriangleMesh> m_EnvironmentTriangles;
    bool bShowEnvironment = true;

    // Shaders
    Shader* m_StaticShader = nullptr;
    Shader* m_SkinnedShader = nullptr;
    
    // Debug
    DebugDrawer* m_CurrentPoseVisual = nullptr;
    bool bShowCurrentPose = true;
    bool bShowIKPose = true;
    bool bDepthTest = false;
    float m_TimeSpeed = 1.f;

    void AdjustCharacterToGround();
    
}; // CharacterIKApp
