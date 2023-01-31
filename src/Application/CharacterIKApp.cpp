#include "Application/CharacterIKApp.h"

#include <iostream>

#include "Animation/AnimationUtilities.h"
#include "Animation/Clip.h"
#include "Animation/FastTrack.h"
#include "Animation/Frame.h"
#include "Animation/Interpolation.h"
#include "Core/BasicUtils.h"
#include "Core/Mat4.h"
#include "glad/glad.h"
#include "GLTF/GLTFLoader.h"
#include "IK/IKLeg.h"
#include "Physics/PhysicsLibrary.h"
#include "Physics/Ray.h"
#include "Render/DebugDrawer.h"
#include "Render/Shader.h"
#include "Render/Texture.h"
#include "Render/Uniform.h"
#include "SkeletalMesh/SkeletalMesh.h"
#include "SkeletalMesh/TriangleMesh.h"

// ---------------------------------------------------------------------------------------------------------------------

CharacterIKApp::CharacterIKApp()
{
    
} // CharacterIKApp

// ---------------------------------------------------------------------------------------------------------------------

void CharacterIKApp::Initialize()
{
    Application::Initialize();

    // Load character's skeletal mesh, clips and texture
    cgltf_data* character_data = GLTFLoader::LoadGLTFFile("Assets/Woman.gltf");
    m_CharacterMeshes = GLTFLoader::LoadSkeletalMeshes(character_data);
    m_Skeleton = GLTFLoader::LoadSkeleton(character_data);
    const std::vector<Clip> clips = GLTFLoader::LoadAnimationClips(character_data);
    GLTFLoader::FreeGLTFFile(character_data);
    
    m_CharacterTexture = new Texture("Assets/Woman.png");
    m_CurrentPose = m_Skeleton.GetRestPose();
    m_CurrentPose.GetMatrixPaletteWithInvPose(m_PreSkinnedPalette, m_Skeleton);

    // Set IK with anim curves
    m_RightLeg = new IKLeg(m_Skeleton, "RightUpLeg", "RightLeg", "RightFoot", "RightToeBase");
    m_RightLeg->SetAnkleOffset(m_AnkleOffset);
    
    ScalarTrack rightTrack;
    rightTrack.SetInterpolation(Interpolation::Cubic);
    rightTrack.Resize(4);
    rightTrack[0].m_Value[0] = 1;
    rightTrack[1].m_Value[0] = 0;
    rightTrack[2].m_Value[0] = 0;
    rightTrack[3].m_Value[0] = 1;
    rightTrack[0].m_Time = 0.0f;
    rightTrack[1].m_Time = 0.3f;
    rightTrack[2].m_Time = 0.7f;
    rightTrack[3].m_Time = 1.0f;
    m_RightLeg->SetPinTrack(rightTrack);

    m_LeftLeg = new IKLeg(m_Skeleton, "LeftUpLeg", "LeftLeg", "LeftFoot", "LeftToeBase");
    m_LeftLeg->SetAnkleOffset(m_AnkleOffset);
   
    ScalarTrack leftTrack;
    leftTrack.SetInterpolation(Interpolation::Cubic);
    leftTrack.Resize(4);
    leftTrack[0].m_Value[0] = 0;
    leftTrack[1].m_Value[0] = 1;
    leftTrack[2].m_Value[0] = 1;
    leftTrack[3].m_Value[0] = 0;
    leftTrack[0].m_Time = 0.0f;
    leftTrack[1].m_Time = 0.4f;
    leftTrack[2].m_Time = 0.6f;
    leftTrack[3].m_Time = 1.0f;
    m_LeftLeg->SetPinTrack(leftTrack);

    // Show skeleton current pose
    m_CurrentPoseVisual = new DebugDrawer();
    m_CurrentPoseVisual->FromPose(m_CurrentPose);
    m_CurrentPoseVisual->UpdateOpenGLBuffers();

    // Animations: [Running, Jump2, PickUp, SitIdle, Idle, Punch, Sitting, Walking, Jump, Lean_Left]
    for (const Clip& clip : clips)
    {
        m_Clips.push_back(AnimationUtilities::OptimizeClip(clip));
    }
    m_CurrentClipIdx = 7; // Walking

    // Create moving track
    m_MotionTrack.Resize(5);
    m_MotionTrack.SetInterpolation(Interpolation::Linear);
    m_MotionTrack[0].m_Time = 0.0f;
    m_MotionTrack[0].m_Value[0] = 0;
    m_MotionTrack[0].m_Value[1] = 0;
    m_MotionTrack[0].m_Value[2] = 1;
    m_MotionTrack[1].m_Time = 1.0f;
    m_MotionTrack[1].m_Value[0] = 0;
    m_MotionTrack[1].m_Value[1] = 0;
    m_MotionTrack[1].m_Value[2] = 10;
    m_MotionTrack[2].m_Time = 3.0f;
    m_MotionTrack[2].m_Value[0] = 22;
    m_MotionTrack[2].m_Value[1] = 0;
    m_MotionTrack[2].m_Value[2] = 10;
    m_MotionTrack[3].m_Time = 4.0f;
    m_MotionTrack[3].m_Value[0] = 22;
    m_MotionTrack[3].m_Value[1] = 0;
    m_MotionTrack[3].m_Value[2] = 2;
    m_MotionTrack[4].m_Time = m_WalkingTotalTime;
    m_MotionTrack[4].m_Value[0] = 0;
    m_MotionTrack[4].m_Value[1] = 0;
    m_MotionTrack[4].m_Value[2] = 1;
    
    // Load environment's meshes and texture
    cgltf_data* environmentData = GLTFLoader::LoadGLTFFile("Assets/IKCourse.gltf");
    m_EnvironmentMeshes = GLTFLoader::LoadStaticMeshes(environmentData);
    GLTFLoader::FreeGLTFFile(environmentData);

    m_EnvironmentTexture = new Texture("Assets/uv.png");
    for (const SkeletalMesh& mesh : m_EnvironmentMeshes)
    {
        mesh.GetTriangles(m_EnvironmentTriangles);
    }

    // Start the character clamped to the ground. Move down a little bit so it's not perfectly up
    AdjustCharacterToGround();
    m_LastHeight = m_Model.position.y;

    // Init IK
    const Vec3 worldLeftAnkle = m_Model.Combine(m_CurrentPose.GetGlobalTransform(m_LeftLeg->GetAnkleIdx())).position;
    const Vec3 worldRightAnkle = m_Model.Combine(m_CurrentPose.GetGlobalTransform(m_RightLeg->GetAnkleIdx())).position;
    m_LeftLeg->SolveForLeg(m_Model, m_CurrentPose, worldLeftAnkle);
    m_RightLeg->SolveForLeg(m_Model, m_CurrentPose, worldRightAnkle);
    
    // Create shaders
    m_StaticShader = new Shader("Shaders/static.vert", "Shaders/lit.frag");
    m_SkinnedShader = new Shader("Shaders/preskinned.vert", "Shaders/lit.frag");
    
} // Initialize

// ---------------------------------------------------------------------------------------------------------------------

void CharacterIKApp::Update(float inDeltaTime)
{
	Application::Update(inDeltaTime);

	inDeltaTime *= m_TimeSpeed;

	// Increment time and sample the animation clip that moves the model on the level rails
	m_WalkingTime += inDeltaTime * m_WalkingSpeed;
	while (m_WalkingTime > m_WalkingTotalTime)
	{
		m_WalkingTime -= m_WalkingTotalTime;
	}

	//m_WalkingTime = 2.25f;
	
	Vec3 currentPosition = m_MotionTrack.Sample(m_WalkingTime, true);
	static constexpr float NEXT_FRAME_DELTA = 0.1f;
	Vec3 nextPosition = m_MotionTrack.Sample(m_WalkingTime + NEXT_FRAME_DELTA, true);

	// Keep the Y position the same as last frame for both to properly orient the model
	const float lastYPosition = m_Model.position.y;
	currentPosition.y = lastYPosition;
	nextPosition.y = lastYPosition;
	m_Model.position = currentPosition;

	// Figure out the forward direction of the model
	const Vec3 direction = (nextPosition - currentPosition).Normalized();
	Quat correctedDir = Quat::LookRotation(direction, {0, 1, 0});
	if ((m_Model.rotation | correctedDir) < 0.0f)
	{
		correctedDir *= -1.0f;
	}

	const float alphaLerp = std::min(1.f, inDeltaTime * 10.0f);
	m_Model.rotation = Quat::NLerp(m_Model.rotation, correctedDir, alphaLerp);
	const Vec3 characterForward = m_Model.rotation * Vec3{0, 0, 1};

	// Figure out the Y position of the model
	AdjustCharacterToGround();

	// Sample the current animation, update the pose visual and figure out where the left
	// and right leg are in their up/down animation cycle
	m_PlaybackTime += inDeltaTime * m_AnimationSpeed;

	//_PlaybackTime = 0.1f;

	const FastClip& currentClip = m_Clips[m_CurrentClipIdx];
	m_PlaybackTime = currentClip.Sample(m_CurrentPose, m_PlaybackTime);
	m_CurrentPoseVisual->FromPose(m_CurrentPose);
	float normalizedTime = (m_PlaybackTime - currentClip.GetStartTime()) / currentClip.GetDuration();
	normalizedTime = BasicUtils::Clamp(normalizedTime, 0.f, 1.f);
	const float leftMotion = m_LeftLeg->GetPinValue(normalizedTime);
	const float rightMotion = m_RightLeg->GetPinValue(normalizedTime);

	// Construct a ray for the left/right ankle, store the world position and the predictive position
	// of the ankle. This is in case the raycasts below don't hit anything.
	Vec3 worldLeftAnkle = m_Model.Combine(m_CurrentPose.GetGlobalTransform(m_LeftLeg->GetAnkleIdx())).position;
	Vec3 worldRightAnkle = m_Model.Combine(m_CurrentPose.GetGlobalTransform(m_RightLeg->GetAnkleIdx())).position;
	Vec3 predictiveLeftAnkle = worldLeftAnkle;
	Vec3 predictiveRightAnkle = worldRightAnkle;

	static const Vec3 UP_OFFSET = {0, 2, 0};
	const Ray leftAnkleRay = {worldLeftAnkle + UP_OFFSET};
	const Ray rightAnkleRay = {worldRightAnkle + UP_OFFSET};

	// Perform some raycasts for the feet, these are done in world space and
	// will define the IK based target points. For each ankle, we need to know
	// the current position (raycast from knee height to the sole of the foot height)
	// and the predictive position (infinite ray cast). The target point will be
	// between these two goals
	Vec3 groundReference = m_Model.position;
	auto CheckPredictiveAnklePos = [&](const TriangleMesh& triangle, const Ray& ankleRay, Vec3& currentAnklePos,
		Vec3& predictiveAnklePos)
	{
		Vec3 hitPoint;
		if (!PhysicsLibrary::RaycastTriangle(ankleRay, triangle, hitPoint))
		{
			return;
		}

		static constexpr float RAY_HEIGHT = 2.1f;
		static constexpr float RAY_HEIGHT_SQR = RAY_HEIGHT * RAY_HEIGHT;
		
		if ((hitPoint - ankleRay.origin).LenSq() < RAY_HEIGHT_SQR)
		{
			currentAnklePos = hitPoint;
			if (hitPoint.y < groundReference.y)
			{
				groundReference = hitPoint - Vec3{0, m_SinkIntoGround, 0.f};
			}
		}
		
		predictiveAnklePos = hitPoint;
	};
	
	for (const TriangleMesh& triangle : m_EnvironmentTriangles)
	{
		CheckPredictiveAnklePos(triangle, leftAnkleRay, worldLeftAnkle, predictiveLeftAnkle);
		CheckPredictiveAnklePos(triangle, rightAnkleRay, worldRightAnkle, predictiveRightAnkle);
	}

	// Lerp the Y position of the mode over a small period of time avoiding popping
	m_Model.position.y = m_LastHeight;
	const float posAlphaLerp = std::min(1.f, inDeltaTime * 10.0f);
	m_Model.position = Vec3::Lerp(m_Model.position, groundReference, posAlphaLerp);
	m_LastHeight = m_Model.position.y;

	// Lerp between fully clamped to the ground, and somewhat clamped to the ground based on the 
	// current phase of the walk cycle
	worldLeftAnkle = Vec3::Lerp(worldLeftAnkle, predictiveLeftAnkle, leftMotion);
	worldRightAnkle = Vec3::Lerp(worldRightAnkle, predictiveRightAnkle, rightMotion);

	// Now that we know the position of the model, as well as the ankle we can solve the feet.
	m_LeftLeg->SolveForLeg(m_Model, m_CurrentPose, worldLeftAnkle/*, worldLeftToe*/);
	m_RightLeg->SolveForLeg(m_Model, m_CurrentPose, worldRightAnkle/*, worldRightToe*/);
	// Apply the solved feet
	Pose::Blend(m_CurrentPose, m_CurrentPose, m_LeftLeg->GetPose(), 1, m_LeftLeg->GetHipIdx());
	Pose::Blend(m_CurrentPose, m_CurrentPose, m_RightLeg->GetPose(), 1, m_RightLeg->GetHipIdx());

	// Fix toes
	auto FixToe = [&, this](IKLeg* leg, float motion)
	{
		const Transform ankleWorld = m_Model.Combine(m_CurrentPose.GetGlobalTransform(leg->GetAnkleIdx()));
		const Vec3 toeWorldPos = m_Model.Combine(m_CurrentPose.GetGlobalTransform(leg->GetToeIdx())).position;

		Vec3 toeTarget = toeWorldPos;
		Vec3 toePredictive = toeWorldPos;
		
		const Vec3 directionOffset = characterForward * m_ToeLength + Vec3{0, 1, 0};
		const Ray toeRay = {Vec3{ankleWorld.position.x, toeWorldPos.y, ankleWorld.position.z} + directionOffset};

		// Next, see if the toes hit anything
		for (const TriangleMesh& triangle : m_EnvironmentTriangles)
		{
			Vec3 hitPoint;
			if (!PhysicsLibrary::RaycastTriangle(toeRay, triangle, hitPoint))
			{
				continue;
			}

			static constexpr float ANKLE_RAY_HEIGHT = 1.1f;
			static constexpr float ANKLE_RAY_HEIGHT_SQ = ANKLE_RAY_HEIGHT * ANKLE_RAY_HEIGHT;
			if ((hitPoint - toeRay.origin).LenSq() < ANKLE_RAY_HEIGHT_SQ)
			{
				toeTarget = hitPoint;
			}
			toePredictive = hitPoint;
		}

		// Retarget toe target and adjust ankle rotation
		toeTarget = Vec3::Lerp(toeTarget, toePredictive, motion);
		const Vec3 ankleToCurrenToe = toeWorldPos - ankleWorld.position;
		const Vec3 ankleToTargetToe = toeTarget - ankleWorld.position;
		
		if ((ankleToCurrenToe | ankleToTargetToe) <= 1E-5f)
		{
			return;
		}
		
		const Quat ankleRotator = Quat::FromTo(ankleToCurrenToe, ankleToTargetToe);
		const Quat worldRotatedAnkle = ankleWorld.rotation * ankleRotator;
		const Quat localRotatedAnkle = worldRotatedAnkle * ankleWorld.rotation.Inverse();
		
		Transform ankleLocal = m_CurrentPose.GetLocalTransform(leg->GetAnkleIdx());
		ankleLocal.rotation = localRotatedAnkle * ankleLocal.rotation;
		m_CurrentPose.SetLocalTransform(leg->GetAnkleIdx(), ankleLocal);
	};
	
	FixToe(m_LeftLeg, leftMotion);
	FixToe(m_RightLeg, rightMotion);

    m_CurrentPose.GetMatrixPaletteWithInvPose(m_PreSkinnedPalette, m_Skeleton);
    
} // Update

// ---------------------------------------------------------------------------------------------------------------------

void CharacterIKApp::Render(float inAspectRatio)
{
    Application::Render(inAspectRatio);

    const Mat4 projection = Mat4::CreatePerspective(60.0f, inAspectRatio, 0.01f, 1000.0f);
    const Mat4 view = Mat4::CreateLookAt({m_Model.position.x, 5.f, m_Model.position.z + 10.f},
        {m_Model.position.x, 3.f, m_Model.position.z}, {0, 1, 0});
    const Mat4 model = m_Model.ToMat4();
    const Mat4 mvp = projection * view * model;
    const Mat4 vp = projection * view;

    // Draw character
    if (bShowCharacter)
    {
        m_SkinnedShader->Bind();
        Uniform<Mat4>::Set(m_SkinnedShader->GetUniform("model"), model);
        Uniform<Mat4>::Set(m_SkinnedShader->GetUniform("view"), view);
        Uniform<Mat4>::Set(m_SkinnedShader->GetUniform("projection"), projection);
        Uniform<Vec3>::Set(m_SkinnedShader->GetUniform("light"), Vec3{1, 1, 1});
        Uniform<Mat4>::Set(m_SkinnedShader->GetUniform("animated"), m_PreSkinnedPalette);
        m_CharacterTexture->Set(m_SkinnedShader->GetUniform("tex0"), 0);
        
        for (SkeletalMesh& mesh : m_CharacterMeshes)
        {
            mesh.Bind(m_SkinnedShader->GetAttribute("position"), m_SkinnedShader->GetAttribute("normal"),
                m_SkinnedShader->GetAttribute("texCoord"), m_SkinnedShader->GetAttribute("weights"),
                m_SkinnedShader->GetAttribute("joints"));
            mesh.Draw();
            mesh.Unbind(m_SkinnedShader->GetAttribute("position"), m_SkinnedShader->GetAttribute("normal"),
                m_SkinnedShader->GetAttribute("texCoord"), m_SkinnedShader->GetAttribute("weights"),
                m_SkinnedShader->GetAttribute("joints"));
        }
        m_CharacterTexture->Unset(0);
        m_SkinnedShader->Unbind();
    }

    // Draw environment (floor etc)
    if (bShowEnvironment)
    {
        m_StaticShader->Bind();
        Uniform<Mat4>::Set(m_StaticShader->GetUniform("model"), Mat4{});
        Uniform<Mat4>::Set(m_StaticShader->GetUniform("view"), view);
        Uniform<Mat4>::Set(m_StaticShader->GetUniform("projection"), projection);
        Uniform<Vec3>::Set(m_StaticShader->GetUniform("light"), Vec3{1, 1, 1});
        m_EnvironmentTexture->Set(m_StaticShader->GetUniform("tex0"), 0);
        
        for (SkeletalMesh& mesh : m_EnvironmentMeshes)
        {
            mesh.Bind(m_StaticShader->GetAttribute("position"), m_StaticShader->GetAttribute("normal"),
                m_StaticShader->GetAttribute("texCoord"), -1, -1);
            mesh.Draw();
            mesh.Unbind(m_StaticShader->GetAttribute("position"), m_StaticShader->GetAttribute("normal"),
                m_StaticShader->GetAttribute("texCoord"), -1, -1);
        }
        
        m_EnvironmentTexture->Unset(0);
        m_StaticShader->Unbind();
    }

    if (!bDepthTest)
    {
        glDisable(GL_DEPTH_TEST);
    }
    
    if (bShowCurrentPose)
    {
        m_CurrentPoseVisual->UpdateOpenGLBuffers();
        m_CurrentPoseVisual->Draw(DebugDrawMode::Lines, {0.f, 0.f, 1.f,}, mvp);
    }

    if (bShowIKPose)
    {
        m_LeftLeg->Draw(vp, {1, 0, 0});
        m_RightLeg->Draw(vp, {0, 1, 0});
    }

    if (!bDepthTest)
    {
        glEnable(GL_DEPTH_TEST);
    }
    
} // Render

// ---------------------------------------------------------------------------------------------------------------------

void CharacterIKApp::Shutdown()
{
    delete m_CurrentPoseVisual;
    delete m_StaticShader;
    delete m_SkinnedShader;
    delete m_CharacterTexture;
    delete m_EnvironmentTexture;
    delete m_LeftLeg;
    delete m_RightLeg;

    m_Clips.clear();
    m_CharacterMeshes.clear();
    m_EnvironmentMeshes.clear();
    
    Application::Shutdown();
    
} // Shutdown

// ---------------------------------------------------------------------------------------------------------------------

void CharacterIKApp::AdjustCharacterToGround()
{
    const Ray groundRay = {Vec3{m_Model.position.x, 11.f, m_Model.position.z}};
    Vec3 hitPoint;
    
    for (const TriangleMesh& triangle : m_EnvironmentTriangles)
    {
        if (PhysicsLibrary::RaycastTriangle(groundRay, triangle, hitPoint))
        {
            m_Model.position = hitPoint - Vec3{0.f, m_SinkIntoGround, 0.f};
            break;
        }
    }
    
} // AdjustCharacterToGround

// ---------------------------------------------------------------------------------------------------------------------

