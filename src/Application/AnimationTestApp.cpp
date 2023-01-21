#include "Application/AnimationTestApp.h"

#include "Animation/Clip.h"
#include "Core/Mat4.h"
#include "Core/Vec3.h"
#include "GLTF/GLTFLoader.h"
#include "Render/DebugDrawer.h"

// ---------------------------------------------------------------------------------------------------------------------

AnimationTestApp::AnimationTestApp() : Application()
{
    
} // AnimationTestApp

// ---------------------------------------------------------------------------------------------------------------------

void AnimationTestApp::Initialize()
{
    cgltf_data* gltf = GLTFLoader::LoadGLTFFile("Assets/Woman.gltf");
    
    m_RestPose = GLTFLoader::LoadRestPose(gltf);
    m_Clips = GLTFLoader::LoadAnimationClips(gltf);
    GLTFLoader::FreeGLTFFile(gltf);

    m_RestPoseVisual = new DebugDrawer();
    m_RestPoseVisual->FromPose(m_RestPose);
    m_RestPoseVisual->UpdateOpenGLBuffers();
    
    // Running, Jump2, PickUp, SitIdle, Idle, Punch, Sitting, Walking, Jump, Lean_Left
    static constexpr int FIRST_ANIM_IDX = 0;
    SwapAnimation(FIRST_ANIM_IDX);

    m_CurrentPoseVisual = new DebugDrawer();
    m_CurrentPoseVisual->FromPose(m_CurrentPose);
    m_CurrentPoseVisual->UpdateOpenGLBuffers();

    bShowBasePose = false;
    
} // Initialize

// ---------------------------------------------------------------------------------------------------------------------

void AnimationTestApp::Update(float deltaTime)
{
    if (m_CurrentClip < 0)
    {
        m_PlaybackTime += deltaTime;
        return;
    }
    
    static bool bChangeAnimation = false;
    if (bChangeAnimation)
    {
        bChangeAnimation = false;
        SwapAnimation((m_CurrentClip + 1) % m_Clips.size());
    }
    
    m_PlaybackTime += deltaTime;
    
    const Clip& currentClip = m_Clips[m_CurrentClip];
    const float clipEndTime = currentClip.GetEndTime();
    if (m_PlaybackTime >= clipEndTime)
    {
        m_PlaybackTime = clipEndTime;
        bChangeAnimation = true;
    }
    
    m_PlaybackTime = m_Clips[m_CurrentClip].Sample(m_CurrentPose, m_PlaybackTime);
    m_CurrentPoseVisual->FromPose(m_CurrentPose);
    
} // Update

// ---------------------------------------------------------------------------------------------------------------------

void AnimationTestApp::Render(float inAspectRatio)
{
    const Mat4 projection = Mat4::CreatePerspective(60.0f, inAspectRatio, 0.01f, 1000.0f);
    static const Mat4 VIEW = Mat4::CreateLookAt({0, 4, 7}, {0, 4, 0}, {0, 1, 0});
    const Mat4 mvp = projection * VIEW; // No model

    if (bShowBasePose)
    {
        m_RestPoseVisual->Draw(DebugDrawMode::Lines, {1, 0, 0}, mvp);
    }
    
    m_CurrentPoseVisual->UpdateOpenGLBuffers();
    m_CurrentPoseVisual->Draw(DebugDrawMode::Lines, {0, 0, 1}, mvp);
    
} // Render

// ---------------------------------------------------------------------------------------------------------------------

void AnimationTestApp::Shutdown()
{
    delete m_RestPoseVisual;
    delete m_CurrentPoseVisual;
    m_Clips.clear();
    
} // Shutdown

// ---------------------------------------------------------------------------------------------------------------------

void AnimationTestApp::SwapAnimation(const std::string& clipName)
{
    m_CurrentClip = -1;
    
    const unsigned int numUIClips = m_Clips.size();
    for (unsigned int i = 0; i < numUIClips; ++i)
    {
        if (m_Clips[i].GetName() == clipName)
        {
            m_CurrentClip = static_cast<int>(i);
            break;
        }
    }
    
    // Reset base pose
    m_PlaybackTime = 0.f;
    m_CurrentPose = m_RestPose;
    
} // SwapAnimation

// ---------------------------------------------------------------------------------------------------------------------

void AnimationTestApp::SwapAnimation(unsigned idx)
{
    if (idx >= m_Clips.size())
    {
        m_CurrentClip = -1;
        return;
    }

    SwapAnimation(m_Clips[idx].GetName());
    
} // SwapAnimation

// ---------------------------------------------------------------------------------------------------------------------
