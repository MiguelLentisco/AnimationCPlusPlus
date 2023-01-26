#include "Application/AdditiveBlendApp.h"

#include "Animation/AnimationUtilities.h"
#include "Animation/Clip.h"
#include "Animation/FastTrack.h"
#include "Animation/TransformTrack.h"
#include "Core/BasicUtils.h"
#include "Core/Mat4.h"
#include "Core/Vec3.h"
#include "GLTF/GLTFLoader.h"
#include "Render/Shader.h"
#include "Render/Texture.h"
#include "Render/Uniform.h"
#include "SkeletalMesh/SkeletalMesh.h"

// ---------------------------------------------------------------------------------------------------------------------

AdditiveBlendApp::AdditiveBlendApp() : Application()
{
    
} // AdditiveBlendApp

// ---------------------------------------------------------------------------------------------------------------------

void AdditiveBlendApp::Initialize()
{
    Application::Initialize();

    cgltf_data* gltf_data = GLTFLoader::LoadGLTFFile("Assets/Woman.gltf");
    m_Meshes = GLTFLoader::LoadMeshes(gltf_data);
    m_Skeleton = GLTFLoader::LoadSkeleton(gltf_data);
    const std::vector<Clip> clips = GLTFLoader::LoadAnimationClips(gltf_data);
	GLTFLoader::FreeGLTFFile(gltf_data);

	const BoneMap boneMap = m_Skeleton.RearrangeSkeleton();
	for (SkeletalMesh& mesh : m_Meshes)
	{
		mesh.RearrangeMesh(boneMap);
		mesh.UpdateOpenGLBuffers();
	}
	
	for (const Clip& clip : clips)
	{
		FastClip optimizedClip = AnimationUtilities::OptimizeClip(clip);
		optimizedClip.RearrangeClip(boneMap);
		m_Clips.emplace_back(optimizedClip);
	}
	
    m_Shader = new Shader("Shaders/preskinned.vert", "Shaders/lit.frag");
    m_Texture = new Texture("Assets/Woman.png");

	m_CurrentPose = m_Skeleton.GetRestPose();
	m_AddPose = m_Skeleton.GetRestPose();

	// Animations: [Running, Jump2, PickUp, SitIdle, Idle, Punch, Sitting, Walking, Jump, Lean_Left]
	m_BaseClipIdx = 7; // Walking
	m_AdditiveClipIdx = 9; // Lean_Left
	m_AddBasePose = AnimationUtilities::MakeAdditivePose(m_Skeleton, m_Clips[m_AdditiveClipIdx]);
	m_Clips[m_AdditiveClipIdx].SetLooping(false);
	
} // Initialize

// ---------------------------------------------------------------------------------------------------------------------

void AdditiveBlendApp::Update(float deltaTime)
{
    Application::Update(deltaTime);

	m_AdditiveTime += m_AdditiveDirection * deltaTime;
	if (m_AdditiveTime < 0.0f || m_AdditiveTime > 1.f)
	{
		m_AdditiveTime = BasicUtils::Clamp(m_AdditiveTime, 0.f, 1.f);
		m_AdditiveDirection *= -1.0f;
	}

	// Update anim and add poses
	m_PlaybackTime = m_Clips[m_BaseClipIdx].Sample(m_CurrentPose, m_PlaybackTime + deltaTime);
	const FastClip& additiveClip = m_Clips[m_AdditiveClipIdx];
    const float time = additiveClip.GetStartTime() + (additiveClip.GetDuration() * m_AdditiveTime);
	m_Clips[m_AdditiveClipIdx].Sample(m_AddPose, time);
	
	static constexpr int ROOT_BONE = -1;
	Pose::Add(m_CurrentPose, m_CurrentPose, m_AddPose, m_AddBasePose, ROOT_BONE);
	
	// Merge pose palette with inverse bind pose
	m_CurrentPose.GetMatrixPalette(m_PreSkinnedPalette);
	const std::vector<Mat4>& invBindPose = m_Skeleton.GetInvBindPose();
	
	const unsigned int numBones = m_PreSkinnedPalette.size();
	for (unsigned int i = 0; i < numBones; ++i)
	{
		m_PreSkinnedPalette[i] *= invBindPose[i];
	}
    
} // Update

// ---------------------------------------------------------------------------------------------------------------------

void AdditiveBlendApp::Render(float inAspectRatio)
{
    Application::Render(inAspectRatio);
	
	static const Mat4 VIEW = Mat4::CreateLookAt({0, 3, 7}, {0, 3, 0}, {0, 1, 0});
	static const Mat4 MODEL;
	const Mat4 projection = Mat4::CreatePerspective(60.0f, inAspectRatio, 0.01f, 1000.0f);
	
	// GPU Skinned Mesh
	m_Shader->Bind();
	Uniform<Mat4>::Set(m_Shader->GetUniform("model"), MODEL);
	Uniform<Mat4>::Set(m_Shader->GetUniform("view"), VIEW);
	Uniform<Mat4>::Set(m_Shader->GetUniform("projection"), projection);
	Uniform<Vec3>::Set(m_Shader->GetUniform("light"), Vec3{1, 1, 1});
	m_Texture->Set(m_Shader->GetUniform("tex0"), 0);
	
	Uniform<Mat4>::Set(m_Shader->GetUniform("animated"), m_PreSkinnedPalette);

	for (SkeletalMesh& mesh : m_Meshes)
	{
		mesh.Bind(m_Shader->GetAttribute("position"), m_Shader->GetAttribute("normal"),
			m_Shader->GetAttribute("texCoord"), m_Shader->GetAttribute("weights"),
			m_Shader->GetAttribute("joints"));
		mesh.Draw();
		mesh.Unbind(m_Shader->GetAttribute("position"), m_Shader->GetAttribute("normal"),
			m_Shader->GetAttribute("texCoord"), m_Shader->GetAttribute("weights"),
			m_Shader->GetAttribute("joints"));
	}
	
	m_Texture->Unset(0);
	m_Shader->Unbind();
	
} // Render

// ---------------------------------------------------------------------------------------------------------------------

void AdditiveBlendApp::Shutdown()
{
	delete m_Shader;
	delete m_Texture;
	m_Clips.clear();
	m_Meshes.clear();
	
    Application::Shutdown();
	
} // Shutdown

// ---------------------------------------------------------------------------------------------------------------------
