#include "Application/SkeletalAnimationApp.h"

#include "Animation/Clip.h"
#include "Core/Mat4.h"
#include "Core/Vec3.h"
#include "GLTF/GLTFLoader.h"
#include "Render/DebugDrawer.h"

// ---------------------------------------------------------------------------------------------------------------------

SkeletalAnimationApp::SkeletalAnimationApp() : Application()
{
    
} // SkeletalAnimationApp

// ---------------------------------------------------------------------------------------------------------------------

void SkeletalAnimationApp::Initialize()
{
    cgltf_data* gltf_data = GLTFLoader::LoadGLTFFile("Assets/Woman.gltf");

    m_Skeleton = GLTFLoader::LoadSkeleton(gltf_data);
    m_Clips = GLTFLoader::LoadAnimationClips(gltf_data);
    GLTFLoader::FreeGLTFFile(gltf_data);

    m_RestPoseVisual = new DebugDrawer();
    m_RestPoseVisual->FromPose(m_Skeleton.GetRestPose());
    m_RestPoseVisual->UpdateOpenGLBuffers();

    m_BindPoseVisual = new DebugDrawer();
    m_BindPoseVisual->FromPose(m_Skeleton.GetBindPose());
    m_BindPoseVisual->UpdateOpenGLBuffers();
    
    // Running, Jump2, PickUp, SitIdle, Idle, Punch, Sitting, Walking, Jump, Lean_Left
    static constexpr int FIRST_ANIM_IDX = 0;
    SwapAnimation(FIRST_ANIM_IDX);

    m_CurrentPoseVisual = new DebugDrawer();
    m_CurrentPoseVisual->FromPose(m_CurrentPose);
    m_CurrentPoseVisual->UpdateOpenGLBuffers();

    bShowBasePoses = true;
    
} // Initialize

// ---------------------------------------------------------------------------------------------------------------------

void SkeletalAnimationApp::Update(float deltaTime)
{
    if (m_CurrentClip < 0)
    {
        return;
    }
    
    m_PlaybackTime += deltaTime;
    
    const Clip& currentClip = m_Clips[m_CurrentClip];
    const float clipEndTime = currentClip.GetEndTime();
    if (m_PlaybackTime >= clipEndTime)
    {
        const float diff = m_PlaybackTime - clipEndTime;
        NextAnimation();
        m_PlaybackTime = diff;
    }
    
    m_PlaybackTime = m_Clips[m_CurrentClip].Sample(m_CurrentPose, m_PlaybackTime);
    m_CurrentPoseVisual->FromPose(m_CurrentPose);
    
} // Update

// ---------------------------------------------------------------------------------------------------------------------

void SkeletalAnimationApp::Render(float inAspectRatio)
{
    const Mat4 projection = Mat4::CreatePerspective(60.0f, inAspectRatio, 0.01f, 1000.0f);
    static const Mat4 VIEW = Mat4::CreateLookAt({0, 4, -7}, {0, 4, 0}, {0, 1, 0});
    const Mat4 mvp = projection * VIEW; // No model

    if (bShowBasePoses)
    {
        m_RestPoseVisual->Draw(DebugDrawMode::Lines, {1, 0, 0}, mvp);
        m_BindPoseVisual->Draw(DebugDrawMode::Lines, {0, 1, 0}, mvp);
    }
    
    m_CurrentPoseVisual->UpdateOpenGLBuffers();
    m_CurrentPoseVisual->Draw(DebugDrawMode::Lines, {0, 0, 1}, mvp);
    
} // Render

// ---------------------------------------------------------------------------------------------------------------------

void SkeletalAnimationApp::Shutdown()
{
    delete m_RestPoseVisual;
    delete m_CurrentPoseVisual;
    m_Clips.clear();
    
} // Shutdown

// ---------------------------------------------------------------------------------------------------------------------

void SkeletalAnimationApp::SwapAnimation(const std::string& clipName)
{
    // Reset animation
    m_PlaybackTime = 0.f;
    m_CurrentPose = m_Skeleton.GetRestPose();
    m_CurrentClip = -1;
    
    const unsigned int numUIClips = m_Clips.size();
    for (unsigned int i = 0; i < numUIClips; ++i)
    {
        if (m_Clips[i].GetName() == clipName)
        {
            m_CurrentClip = static_cast<int>(i);
            return;
        }
    }
    
} // SwapAnimation

// ---------------------------------------------------------------------------------------------------------------------

void SkeletalAnimationApp::SwapAnimation(unsigned idx)
{
    // Reset animation
    m_PlaybackTime = 0.f;
    m_CurrentPose = m_Skeleton.GetRestPose();
    m_CurrentClip = idx >= m_Clips.size() ? -1 : static_cast<int>(idx);
    
} // SwapAnimation

// ---------------------------------------------------------------------------------------------------------------------

void SkeletalAnimationApp::NextAnimation()
{
    SwapAnimation((m_CurrentClip + 1) % m_Clips.size());
    
} // NextAnimation

// ---------------------------------------------------------------------------------------------------------------------
