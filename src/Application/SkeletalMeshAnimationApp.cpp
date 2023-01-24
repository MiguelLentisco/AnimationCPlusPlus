#include "Application/SkeletalMeshAnimationApp.h"

#include "Animation/Clip.h"
#include "Animation/FastTrack.h"
#include "Animation/TransformTrack.h"
#include "Core/Mat4.h"
#include "GLTF/GLTFLoader.h"
#include "Render/Shader.h"
#include "Render/Texture.h"
#include "Render/Uniform.h"
#include "SkeletalMesh/SkeletalMesh.h"

// ---------------------------------------------------------------------------------------------------------------------

void AnimationInstance::Update(float deltaTime)
{
	UpdateAnimation(deltaTime);
	
	// Merge pose palette with inverse bind pose
	m_AnimatedPose.GetMatrixPalette(m_PosePalette);
	const std::vector<Mat4>& invBindPose = m_Skeleton->GetInvBindPose();
	
	const unsigned int numBones = m_PosePalette.size();
	for (unsigned int i = 0; i < numBones; ++i)
	{
		m_PosePalette[i] *= invBindPose[i];
	}
	
} // Update

// ---------------------------------------------------------------------------------------------------------------------

void AnimationInstance::SwapAnimation(const std::string& clipName)
{
	// Reset animation
	m_PlaybackTime = 0.f;
	m_AnimatedPose = m_Skeleton->GetRestPose();
	m_CurrentClip = -1;
    
	const unsigned int numUIClips = m_Clips->size();
	for (unsigned int i = 0; i < numUIClips; ++i)
	{
		if ((*m_Clips)[i].GetName() == clipName)
		{
			m_CurrentClip = static_cast<int>(i);
			return;
		}
	}
	
} // SwapAnimation

// ---------------------------------------------------------------------------------------------------------------------

void AnimationInstance::SwapAnimation(unsigned idx)
{
	// Reset animation
	m_PlaybackTime = 0.f;
	m_AnimatedPose = m_Skeleton->GetRestPose();
	m_CurrentClip = idx >= m_Clips->size() ? -1 : static_cast<int>(idx);
	
} // SwapAnimation

// ---------------------------------------------------------------------------------------------------------------------

void AnimationInstance::NextAnimation()
{
	SwapAnimation((m_CurrentClip + 1) % m_Clips->size());
	
} // NextAnimation

// ---------------------------------------------------------------------------------------------------------------------

void AnimationInstance::UpdateAnimation(float deltaTime)
{
	if (m_CurrentClip < 0)
	{
		return;
	}
    
	m_PlaybackTime += m_PlaybackSpeed * deltaTime;
    
	const FastClip& currentClip = (*m_Clips)[m_CurrentClip];
	const float clipEndTime = currentClip.GetEndTime();
	if (!bLoop && m_PlaybackTime >= clipEndTime)
	{
		const float diff = m_PlaybackTime - clipEndTime;
		NextAnimation();
		m_PlaybackTime = diff;
	}
	
	m_PlaybackTime = currentClip.Sample(m_AnimatedPose, m_PlaybackTime);
	
} // UpdateAnimation

// ---------------------------------------------------------------------------------------------------------------------

SkeletalMeshAnimationApp::SkeletalMeshAnimationApp() : Application()
{
    
} // SkeletalMeshAnimationApp

// ---------------------------------------------------------------------------------------------------------------------

void SkeletalMeshAnimationApp::Initialize()
{
    Application::Initialize();

    cgltf_data* gltf_data = GLTFLoader::LoadGLTFFile("Assets/Woman.gltf");
    m_CPUMeshes = GLTFLoader::LoadMeshes(gltf_data);
    m_Skeleton = GLTFLoader::LoadSkeleton(gltf_data);
    const std::vector<Clip> clips = GLTFLoader::LoadAnimationClips(gltf_data);
	GLTFLoader::FreeGLTFFile(gltf_data);
	
	for (const Clip& clip : clips)
	{
		m_Clips.emplace_back(ClipUtilities::OptimizeClip(clip));
	}

    m_GPUMeshes = m_CPUMeshes;
    const unsigned int numMeshes = m_GPUMeshes.size();
    for (unsigned int i = 0; i < numMeshes; ++i)
    {
        m_GPUMeshes[i].UpdateOpenGLBuffers();
    }

    m_StaticShader = new Shader("Shaders/static.vert", "Shaders/lit.frag");
    m_SkinnedShader = new Shader("Shaders/preskinned.vert", "Shaders/lit.frag");
    m_DiffuseTexture = new Texture("Assets/Woman.png");
	
    m_GPUAnimInfo.m_AnimatedPose = m_Skeleton.GetRestPose();
    m_GPUAnimInfo.m_PosePalette.resize(m_Skeleton.GetRestPose().GetSize());
	m_GPUAnimInfo.m_Clips = &m_Clips;
	m_GPUAnimInfo.m_Skeleton = &m_Skeleton;
    m_CPUAnimInfo.m_AnimatedPose = m_Skeleton.GetRestPose();
    m_CPUAnimInfo.m_PosePalette.resize(m_Skeleton.GetRestPose().GetSize());
	m_CPUAnimInfo.m_Clips = &m_Clips;
	m_CPUAnimInfo.m_Skeleton = &m_Skeleton;

    m_GPUAnimInfo.m_Model.position = {-2, 0, 0};
    m_CPUAnimInfo.m_Model.position = {2, 0, 0};

	// Running, Jump2, PickUp, SitIdle, Idle, Punch, Sitting, Walking, Jump, Lean_Left
	static constexpr int GPU_FIRST_ANIM_IDX = 0;
	static constexpr int CPU_FIRST_ANIM_IDX = 5;
	m_GPUAnimInfo.SwapAnimation(GPU_FIRST_ANIM_IDX);
	m_CPUAnimInfo.SwapAnimation(CPU_FIRST_ANIM_IDX);
    
} // Initialize

// ---------------------------------------------------------------------------------------------------------------------

void SkeletalMeshAnimationApp::Update(float deltaTime)
{
    Application::Update(deltaTime);
	
	m_CPUAnimInfo.Update(deltaTime);
	m_GPUAnimInfo.Update(deltaTime);

	const unsigned int numCPUMeshes = m_CPUMeshes.size();
    for (unsigned int i = 0; i < numCPUMeshes; ++i)
    {
        m_CPUMeshes[i].CPUSkin(m_CPUAnimInfo.m_PosePalette);
    }
    
} // Update

// ---------------------------------------------------------------------------------------------------------------------

void SkeletalMeshAnimationApp::Render(float inAspectRatio)
{
    Application::Render(inAspectRatio);

    const Mat4 projection = Mat4::CreatePerspective(60.0f, inAspectRatio, 0.01f, 1000.0f);
	static const Mat4 VIEW = Mat4::CreateLookAt({0, 5, 7}, {0, 3, 0}, {0, 1, 0});

	// CPU Skinned Mesh
	m_StaticShader->Bind();
	Uniform<Mat4>::Set(m_StaticShader->GetUniform("model"), m_CPUAnimInfo.m_Model.ToMat4());
	Uniform<Mat4>::Set(m_StaticShader->GetUniform("view"), VIEW);
	Uniform<Mat4>::Set(m_StaticShader->GetUniform("projection"), projection);
	Uniform<Vec3>::Set(m_StaticShader->GetUniform("light"), Vec3{1, 1, 1});
	m_DiffuseTexture->Set(m_StaticShader->GetUniform("tex0"), 0);

	const unsigned int numCPUMeshes = m_CPUMeshes.size();
	for (unsigned int i = 0; i < numCPUMeshes; ++i)
	{
		m_CPUMeshes[i].Bind(m_StaticShader->GetAttribute("position"), m_StaticShader->GetAttribute("normal"),
			m_StaticShader->GetAttribute("texCoord"), -1, -1);
		m_CPUMeshes[i].Draw();
		m_CPUMeshes[i].Unbind(m_StaticShader->GetAttribute("position"), m_StaticShader->GetAttribute("normal"),
			m_StaticShader->GetAttribute("texCoord"), -1, -1);
	}
	
	m_DiffuseTexture->Unset(0);
	m_StaticShader->Unbind();
	
	// GPU Skinned Mesh
	m_SkinnedShader->Bind();
	Uniform<Mat4>::Set(m_SkinnedShader->GetUniform("model"), m_GPUAnimInfo.m_Model.ToMat4());
	Uniform<Mat4>::Set(m_SkinnedShader->GetUniform("view"), VIEW);
	Uniform<Mat4>::Set(m_SkinnedShader->GetUniform("projection"), projection);
	Uniform<Vec3>::Set(m_SkinnedShader->GetUniform("light"), Vec3{1, 1, 1});
	m_DiffuseTexture->Set(m_SkinnedShader->GetUniform("tex0"), 0);
	
	Uniform<Mat4>::Set(m_SkinnedShader->GetUniform("animated"), m_GPUAnimInfo.m_PosePalette);

	const unsigned int numGPUMeshes = m_GPUMeshes.size();
	for (unsigned int i = 0; i < numGPUMeshes; ++i)
	{
		m_GPUMeshes[i].Bind(m_SkinnedShader->GetAttribute("position"), m_SkinnedShader->GetAttribute("normal"),
			m_SkinnedShader->GetAttribute("texCoord"), m_SkinnedShader->GetAttribute("weights"),
			m_SkinnedShader->GetAttribute("joints"));
		m_GPUMeshes[i].Draw();
		m_GPUMeshes[i].Unbind(m_SkinnedShader->GetAttribute("position"), m_SkinnedShader->GetAttribute("normal"),
			m_SkinnedShader->GetAttribute("texCoord"), m_SkinnedShader->GetAttribute("weights"),
			m_SkinnedShader->GetAttribute("joints"));
	}
	
	m_DiffuseTexture->Unset(0);
	m_SkinnedShader->Unbind();
	
} // Render

// ---------------------------------------------------------------------------------------------------------------------

void SkeletalMeshAnimationApp::Shutdown()
{
	delete m_StaticShader;
	delete m_DiffuseTexture;
	delete m_SkinnedShader;
	m_Clips.clear();
	m_CPUMeshes.clear();
	m_GPUMeshes.clear();
	
    Application::Shutdown();
	
} // Shutdown

// ---------------------------------------------------------------------------------------------------------------------
