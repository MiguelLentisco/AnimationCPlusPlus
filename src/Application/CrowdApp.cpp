#include "Application/CrowdApp.h"

#include <io.h> 
#define access _access_s

#include <iostream>

#include "Animation/AnimationUtilities.h"
#include "Animation/Clip.h"
#include "Animation/Crowd.h"
#include "Core/Mat4.h"
#include "Core/Vec3.h"
#include "GLTF/GLTFLoader.h"
#include "Render/AnimTexture.h"
#include "Render/Shader.h"
#include "Render/Texture.h"
#include "Render/Uniform.h"
#include "SkeletalMesh/SkeletalMesh.h"
#include "Animation/FastTrack.h"
#include "Animation/TransformTrack.h"
#include "Core/TVec2.h"
#include "Core/Quat.h"

// ---------------------------------------------------------------------------------------------------------------------

CrowdApp::CrowdApp() : Application()
{
    
} // CrowdApp

// ---------------------------------------------------------------------------------------------------------------------


void CrowdApp::Initialize()
{
    Application::Initialize();

    cgltf_data* gltf = GLTFLoader::LoadGLTFFile("Assets/Woman.gltf");
    m_Meshes = GLTFLoader::LoadSkeletalMeshes(gltf);
    m_Skeleton = GLTFLoader::LoadSkeleton(gltf);
    const std::vector<Clip> clips = GLTFLoader::LoadAnimationClips(gltf);
    GLTFLoader::FreeGLTFFile(gltf);

    const BoneMap boneMap = m_Skeleton.RearrangeSkeleton();
    for (SkeletalMesh& mesh : m_Meshes)
    {
        mesh.RearrangeMesh(boneMap);
    }
    
    // Animations: [Running, Jump2, PickUp, SitIdle, Idle, Punch, Sitting, Walking, Jump, Lean_Left]
    for (const Clip& clip : clips)
    {
        FastClip optimizedClip = AnimationUtilities::OptimizeClip(clip);
        optimizedClip.RearrangeClip(boneMap);
        m_Clips.emplace_back(optimizedClip);
    }

    m_CrowdShader = new Shader("Shaders/crowd.vert", "Shaders/lit.frag");
    m_DiffuseTexture = new Texture("Assets/Woman.png");

    const unsigned int numClips = m_Clips.size();
    m_AnimTextures.resize(numClips);
    m_Crowds.resize(numClips);
    
    for (unsigned int i = 0; i < numClips; ++i)
    {
        std::string fileName = "Assets/";
        fileName += m_Clips[i].GetName();
        fileName += ".animTex";
        const bool fileExists = access(fileName.c_str(), 0) == 0;

        if (fileExists)
        {
            m_AnimTextures[i].Load(fileName.c_str());
        }
        else
        {
            m_AnimTextures[i].Resize(512);
            AnimationUtilities::BakeAnimationToTexture(m_Skeleton, m_Clips[i], m_AnimTextures[i]);
            m_AnimTextures[i].Save(fileName.c_str());
        }
    }

    SetCrowdSize(20);
    
} // Initialize

// ---------------------------------------------------------------------------------------------------------------------

void CrowdApp::Update(float deltaTime)
{
    Application::Update(deltaTime);

    const unsigned int numCrowds = m_Crowds.size();
    for (unsigned int i = 0; i < numCrowds; ++i)
    {
        m_Crowds[i].Update(deltaTime, m_Clips[i], m_AnimTextures[i].GetSize());
    }
    
} // Update

// ---------------------------------------------------------------------------------------------------------------------

void CrowdApp::Render(float inAspectRatio)
{
    Application::Render(inAspectRatio);

    const Mat4 projection = Mat4::CreatePerspective(60.0f, inAspectRatio, 0.01f, 1000.0f);
    static const Mat4 VIEW = Mat4::CreateLookAt(Vec3{0, 15, 40}, Vec3{0, 3, 0}, Vec3{0, 1, 0});

    m_CrowdShader->Bind();
    Uniform<Mat4>::Set(m_CrowdShader->GetUniform("view"), VIEW);
    Uniform<Mat4>::Set(m_CrowdShader->GetUniform("projection"), projection);
    Uniform<Vec3>::Set(m_CrowdShader->GetUniform("light"), Vec3{1, 1, 1});

    Uniform<Mat4>::Set(m_CrowdShader->GetUniform("invBindPose"), m_Skeleton.GetInvBindPose());
    m_DiffuseTexture->Set(m_CrowdShader->GetUniform("tex0"), 0);

    const unsigned int numCrowds = m_Crowds.size();
    for (unsigned int c = 0; c < numCrowds; ++c)
    {
        m_AnimTextures[c].Set(m_CrowdShader->GetUniform("animTex"), 1);
        m_Crowds[c].SetUniforms(m_CrowdShader);

        for (const SkeletalMesh& mesh : m_Meshes)
        {
            mesh.Bind(m_CrowdShader->GetAttribute("position"), m_CrowdShader->GetAttribute("normal"),
                m_CrowdShader->GetAttribute("texCoord"), m_CrowdShader->GetAttribute("weights"),
                m_CrowdShader->GetAttribute("joints"));
            mesh.DrawInstanced(m_Crowds[c].GetSize());
            mesh.Unbind(m_CrowdShader->GetAttribute("position"), m_CrowdShader->GetAttribute("normal"),
                m_CrowdShader->GetAttribute("texCoord"), m_CrowdShader->GetAttribute("weights"),
                m_CrowdShader->GetAttribute("joints"));
        }
        
        m_AnimTextures[c].Unset(1);
    }

    m_DiffuseTexture->Unset(0);
    m_CrowdShader->Unbind();
    
} // Render

// ---------------------------------------------------------------------------------------------------------------------

void CrowdApp::Shutdown()
{
    delete m_DiffuseTexture;
    delete m_CrowdShader;
    m_Meshes.clear();
    m_Clips.clear();
    m_AnimTextures.clear();
    m_Crowds.clear();
    
    Application::Shutdown();
    
} // Shutdown

// ---------------------------------------------------------------------------------------------------------------------

void CrowdApp::SetCrowdSize(unsigned size)
{
    const unsigned int numCrowds = m_Crowds.size();
    for (unsigned int i = 0; i < numCrowds; ++i)
    {
        m_Crowds[i].Resize(20);
        m_Crowds[i].RandomizeTimes(m_Clips[i]);
        m_Crowds[i].RandomizePositions(Vec3{-40, 0, -80.0f}, Vec3{40, 0, 30.0f}, 2.0f);
    }
    
} // SetCrowdSize

// ---------------------------------------------------------------------------------------------------------------------

