#include "Application/SimpleBlendApp.h"

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

template <typename TRACK>
void SimpleBlendAnimationInstance::Update(const std::vector<TClip<TRACK>>& clips, float deltaTime)
{
	m_Time = clips[m_CurrentClip].Sample(m_Pose, m_Time + m_PlaybackSpeed * deltaTime);
	
} // Update

// ---------------------------------------------------------------------------------------------------------------------

SimpleBlendApp::SimpleBlendApp() : Application()
{
    
} // SimpleBlendApp

// ---------------------------------------------------------------------------------------------------------------------

void SimpleBlendApp::Initialize()
{
    Application::Initialize();

    cgltf_data* gltf_data = GLTFLoader::LoadGLTFFile("Assets/Woman.gltf");
    m_Meshes = GLTFLoader::LoadSkeletalMeshes(gltf_data);
    m_Skeleton = GLTFLoader::LoadSkeleton(gltf_data);
    const std::vector<Clip> clips = GLTFLoader::LoadAnimationClips(gltf_data);
	GLTFLoader::FreeGLTFFile(gltf_data);

	const BoneMap boneMap = m_Skeleton.RearrangeSkeleton();
	for (SkeletalMesh& mesh : m_Meshes)
	{
		mesh.RearrangeMesh(boneMap);
		mesh.UpdateOpenGLBuffers();
	}
	
	m_FinalPose = m_Skeleton.GetRestPose();
	m_FinalPose.GetMatrixPaletteWithInvPose(m_PreSkinnedPalette, m_Skeleton);
	
	for (const Clip& clip : clips)
	{
		FastClip optimizedClip = AnimationUtilities::OptimizeClip(clip);
		optimizedClip.RearrangeClip(boneMap);
		m_Clips.emplace_back(optimizedClip);
	}
	
    m_Shader = new Shader("Shaders/preskinned.vert", "Shaders/lit.frag");
    m_Texture = new Texture("Assets/Woman.png");
	
	// Animations: [Running, Jump2, PickUp, SitIdle, Idle, Punch, Sitting, Walking, Jump, Lean_Left]
	m_A1.m_Pose = m_FinalPose;
	m_A1.m_CurrentClip = 7;
	m_A1.m_PlaybackSpeed = 1.f;

	m_A2.m_Pose = m_FinalPose;
	m_A2.m_CurrentClip = 0;
	m_A2.m_PlaybackSpeed = 1.f;
	
} // Initialize

// ---------------------------------------------------------------------------------------------------------------------

void SimpleBlendApp::Update(float deltaTime)
{
    Application::Update(deltaTime);

	// Update anims and blend
	m_A1.Update(m_Clips, deltaTime);
	m_A2.Update(m_Clips, deltaTime);

	static constexpr float MAX_BLEND_TIME = 1.f;
	float alpha = std::min(m_BlendTime / MAX_BLEND_TIME, 1.f);
	if (bInvertBlend)
	{
		alpha = 1.f - alpha;
	}

	static constexpr int ROOT_BONE = -1;
	Pose::Blend(m_FinalPose, m_A1.m_Pose, m_A2.m_Pose, alpha, ROOT_BONE);
	
	// Merge pose palette with inverse bind pose
	m_FinalPose.GetMatrixPaletteWithInvPose(m_PreSkinnedPalette, m_Skeleton);
	const std::vector<Mat4>& invBindPose = m_Skeleton.GetInvBindPose();
	
	m_BlendTime += deltaTime;

	static constexpr float SWAP_ANIM_TIME = 2.f;
	if (m_BlendTime >= SWAP_ANIM_TIME)
	{
		m_BlendTime = 0.0f;
		m_FinalPose = m_Skeleton.GetRestPose();
		bInvertBlend = !bInvertBlend;
	}
    
} // Update

// ---------------------------------------------------------------------------------------------------------------------

void SimpleBlendApp::Render(float inAspectRatio)
{
    Application::Render(inAspectRatio);
	
	static const Mat4 VIEW = Mat4::CreateLookAt({0, 5, 5}, {0, 3, 0}, {0, 1, 0});
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

void SimpleBlendApp::Shutdown()
{
	delete m_Shader;
	delete m_Texture;
	m_Clips.clear();
	m_Meshes.clear();
	
    Application::Shutdown();
	
} // Shutdown

// ---------------------------------------------------------------------------------------------------------------------
