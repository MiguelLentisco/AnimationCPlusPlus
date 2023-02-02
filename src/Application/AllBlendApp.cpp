#include "Application/AllBlendApp.h"

#include <random>

#include "Animation/AnimationUtilities.h"
#include "Animation/Clip.h"
#include "Animation/FastTrack.h"
#include "Animation/TransformTrack.h"
#include "Blend/CrossFadeTarget.h"
#include "Core/BasicUtils.h"
#include "Core/Mat4.h"
#include "Core/Vec3.h"
#include "GLTF/GLTFLoader.h"
#include "Render/Shader.h"
#include "Render/Texture.h"
#include "Render/Uniform.h"
#include "SkeletalMesh/SkeletalMesh.h"

// ---------------------------------------------------------------------------------------------------------------------

AllBlendApp::AllBlendApp() : Application()
{
    
} // AllBlendApp

// ---------------------------------------------------------------------------------------------------------------------

void AllBlendApp::Initialize()
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
	}
	
	for (const Clip& clip : clips)
	{
		FastClip optimizedClip = AnimationUtilities::OptimizeClip(clip);
		optimizedClip.RearrangeClip(boneMap);
		m_Clips.emplace_back(optimizedClip);
	}
	
    m_Shader = new Shader("Shaders/preskinned.vert", "Shaders/lit.frag");
    m_Texture = new Texture("Assets/Woman.png");
	
	// Animations: [Running, Jump2, PickUp, SitIdle, Idle, Punch, Sitting, Walking, Jump, Lean_Left]
	static constexpr unsigned int INITIAL_CLIP = 6;
	m_FadeController.SetSkeleton(m_Skeleton);
	m_FadeController.Play(&m_Clips[INITIAL_CLIP]);
	m_FadeController.Update(0.0f);
	m_FadeController.GetCurrentPose().GetMatrixPreSkinnedPalette(m_PreSkinnedPalette, m_Skeleton);
	
} // Initialize

// ---------------------------------------------------------------------------------------------------------------------

void AllBlendApp::Update(float deltaTime)
{
    Application::Update(deltaTime);

	// Update anim controller
	m_FadeController.Update(deltaTime);

	m_FadeOutTimer -= deltaTime;
	if (m_FadeOutTimer < 0.f)
	{
		m_FadeOutTimer = m_FadeOutCooldown;

		// https://en.cppreference.com/w/cpp/numeric/random/uniform_int_distribution
		static std::random_device rd;  //Will be used to obtain a seed for the random number engine
		static std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
		static std::uniform_int_distribution<unsigned> uniformDist(0, m_Clips.size() - 1);
		
		unsigned int clip = uniformDist(gen);
		while (clip == m_CurrentClip)
		{
			clip = uniformDist(gen);
		}
		m_FadeController.FadeTo(&m_Clips[clip], m_FadeOutTime);
	}
	
	// Merge pose palette with inverse bind pose
	m_FadeController.GetCurrentPose().GetMatrixPreSkinnedPalette(m_PreSkinnedPalette, m_Skeleton);
    
} // Update

// ---------------------------------------------------------------------------------------------------------------------

void AllBlendApp::Render(float inAspectRatio)
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

void AllBlendApp::Shutdown()
{
	delete m_Shader;
	delete m_Texture;
	m_Clips.clear();
	m_Meshes.clear();
	
    Application::Shutdown();
	
} // Shutdown

// ---------------------------------------------------------------------------------------------------------------------
