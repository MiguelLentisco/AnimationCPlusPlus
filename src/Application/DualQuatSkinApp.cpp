#include "Application/DualQuatSkinApp.h"

#include "Animation/AnimationUtilities.h"
#include "Animation/Clip.h"
#include "Animation/FastTrack.h"
#include "Animation/TransformTrack.h"
#include "Core/DualQuaternion.h"
#include "Core/Mat4.h"
#include "Core/Transform.h"
#include "GLTF/GLTFLoader.h"
#include "Render/Shader.h"
#include "Render/Texture.h"
#include "Render/Uniform.h"
#include "SkeletalMesh/SkeletalMesh.h"

// ---------------------------------------------------------------------------------------------------------------------

DualQuatSkinApp::DualQuatSkinApp() : Application()
{
    
} // DualQuatSkinApp

// ---------------------------------------------------------------------------------------------------------------------

void DualQuatSkinApp::Initialize()
{
    Application::Initialize();

    cgltf_data* gltf = GLTFLoader::LoadGLTFFile("Assets/dq.gltf");
    m_Meshes = GLTFLoader::LoadSkeletalMeshes(gltf);
    m_Skeleton = GLTFLoader::LoadSkeleton(gltf);
    const std::vector<Clip> clips = GLTFLoader::LoadAnimationClips(gltf);
    GLTFLoader::FreeGLTFFile(gltf);

	for (const Clip& clip : clips)
	{
		m_Clips.emplace_back(AnimationUtilities::OptimizeClip(clip));
	}

    m_LBSShader = new Shader("Shaders/preskinned.vert", "Shaders/lit.frag");
    m_DQShader = new Shader("Shaders/dualquat_skinned.vert", "Shaders/lit.frag");
    m_DiffuseTexture = new Texture("Assets/dq.png");

    m_CurrentPose = m_Skeleton.GetRestPose();
	m_Skeleton.GetInvBindPose(m_DQInvBindPalette);
    m_CurrentPose.GetDualQuaternionPalette(m_DQPosePalette);
    m_CurrentPose.GetMatrixPreSkinnedPalette(m_LBPreSkinnedPalette, m_Skeleton);
    
} // Initialize

// ---------------------------------------------------------------------------------------------------------------------

void DualQuatSkinApp::Update(float deltaTime)
{
    Application::Update(deltaTime);

	m_PlaybackTime += deltaTime;
    m_PlaybackTime = m_Clips[m_CurrentClip].Sample(m_CurrentPose, m_PlaybackTime);
	
	m_CurrentPose.GetDualQuaternionPalette(m_DQPosePalette);
	m_CurrentPose.GetMatrixPreSkinnedPalette(m_LBPreSkinnedPalette, m_Skeleton);
    
} // Update

// ---------------------------------------------------------------------------------------------------------------------

void DualQuatSkinApp::Render(float inAspectRatio)
{
    Application::Render(inAspectRatio);

    const Mat4 projection = Mat4::CreatePerspective(60.0f, inAspectRatio, 0.01f, 1000.0f);
	static const Mat4 VIEW = Mat4::CreateLookAt({0, 3, 14}, {0, 0, 0}, {0, 1, 0});
	
	// Dual Quaternion
	Transform model1({2, 2, 0});
	
	m_DQShader->Bind();
	Uniform<Mat4>::Set(m_DQShader->GetUniform("model"), model1.ToMat4());
	Uniform<Mat4>::Set(m_DQShader->GetUniform("view"), VIEW);
	Uniform<Mat4>::Set(m_DQShader->GetUniform("projection"), projection);
	Uniform<Vec3>::Set(m_DQShader->GetUniform("light"), Vec3{1, 1, 1});
	Uniform<DualQuaternion>::Set(m_DQShader->GetUniform("pose"), m_DQPosePalette);
	Uniform<DualQuaternion>::Set(m_DQShader->GetUniform("invBindPose"), m_DQInvBindPalette);
	m_DiffuseTexture->Set(m_DQShader->GetUniform("tex0"), 0);
	
	for (const SkeletalMesh& mesh : m_Meshes)
	{
		mesh.Bind(m_DQShader->GetAttribute("position"), m_DQShader->GetAttribute("normal"),
			m_DQShader->GetAttribute("texCoord"), m_DQShader->GetAttribute("weights"),
			m_DQShader->GetAttribute("joints"));
		mesh.Draw();
		mesh.Unbind(m_DQShader->GetAttribute("position"), m_DQShader->GetAttribute("normal"),
			m_DQShader->GetAttribute("texCoord"), m_DQShader->GetAttribute("weights"),
			m_DQShader->GetAttribute("joints"));
	}

	m_DiffuseTexture->Unset(0);
	m_DQShader->Unbind();

	// Linear Blend
	Transform model2({-2, -2, 0});
	
	m_LBSShader->Bind();
	Uniform<Mat4>::Set(m_LBSShader->GetUniform("model"), model2.ToMat4());
	Uniform<Mat4>::Set(m_LBSShader->GetUniform("view"), VIEW);
	Uniform<Mat4>::Set(m_LBSShader->GetUniform("projection"), projection);
	Uniform<Vec3>::Set(m_LBSShader->GetUniform("light"), Vec3{1, 1, 1});
	Uniform<Mat4>::Set(m_LBSShader->GetUniform("animated"), m_LBPreSkinnedPalette);

	m_DiffuseTexture->Set(m_LBSShader->GetUniform("tex0"), 0);
	for (const SkeletalMesh& mesh : m_Meshes)
	{
		mesh.Bind(m_LBSShader->GetAttribute("position"), m_LBSShader->GetAttribute("normal"),
			m_LBSShader->GetAttribute("texCoord"), m_LBSShader->GetAttribute("weights"),
			m_LBSShader->GetAttribute("joints"));
		mesh.Draw();
		mesh.Unbind(m_LBSShader->GetAttribute("position"), m_LBSShader->GetAttribute("normal"),
			m_LBSShader->GetAttribute("texCoord"), m_LBSShader->GetAttribute("weights"),
			m_LBSShader->GetAttribute("joints"));
	}
	
	m_DiffuseTexture->Unset(0);
	m_LBSShader->Unbind();
    
} // Render

// ---------------------------------------------------------------------------------------------------------------------

void DualQuatSkinApp::Shutdown()
{
	delete m_DiffuseTexture;
	delete m_LBSShader;
	delete m_DQShader;
	m_Meshes.clear();
	m_Clips.clear();
	
    Application::Shutdown();
    
} // Shutdown

// ---------------------------------------------------------------------------------------------------------------------

