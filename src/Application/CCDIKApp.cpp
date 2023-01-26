#include "Application/CCDIKApp.h"

#include "Animation/Frame.h"
#include "Core/BasicUtils.h"
#include "Core/Mat4.h"
#include "Render/DebugDrawer.h"

// ---------------------------------------------------------------------------------------------------------------------

void CCDIKApp::Initialize()
{
    Application::Initialize();

    m_Solver.Resize(6);
    m_Solver[0] = Transform(Vec3(), Quat::CreateFromAxis(BasicUtils::DegToRad(90.f), Vec3(1, 0, 0)), Vec3(1, 1, 1));
    m_Solver[1] = Transform(Vec3(0, 0, 1.0f), {}, Vec3(1, 1, 1));
    m_Solver[2] = Transform(Vec3(0, 0, 1.5f), {}, Vec3(1, 1, 1));
    m_Solver[3] = Transform(Vec3(0, 0, 0.5f), Quat::CreateFromAxis(BasicUtils::DegToRad(90.f), Vec3(0, 1, 0)), Vec3(1, 1, 1));
    m_Solver[4] = Transform(Vec3(0, 0, 0.5f), {}, Vec3(1, 1, 1));
    m_Solver[5] = Transform(Vec3(0, 0, 0.5f), {}, Vec3(1, 1, 1));

    m_Target.position = {1, -2, 0};

    // Create manual movement of target position
    VectorTrack& path = m_TargetPath.GetPositionTrack();
    path.Resize(14);
    SetFrame(path, 0, 0.0f, Vec3(1, -2, 0) * 0.5f);
    SetFrame(path, 1, 1.0f, Vec3(1, 2, 0) * 0.5f);
    SetFrame(path, 2, 2.0f, Vec3(1, 4, 0) * 0.5f);
    SetFrame(path, 3, 3.0f, Vec3(3, 4, 0) * 0.5f);
    SetFrame(path, 4, 4.0f, Vec3(5, 4, 0) * 0.5f);
    SetFrame(path, 5, 5.0f, Vec3(5, 4, 2) * 0.5f);
    SetFrame(path, 6, 6.0f, Vec3(5, 4, 4) * 0.5f);
    SetFrame(path, 7, 7.0f, Vec3(3, 4, 4) * 0.5f);
    SetFrame(path, 8, 8.0f, Vec3(3, 2, 4) * 0.5f);
    SetFrame(path, 9, 9.0f, Vec3(3, 2, 2) * 0.5f);
    SetFrame(path, 10, 10.0f, Vec3(1, 2, 2) * 0.5f);
    SetFrame(path, 11, 11.0f, Vec3(1, 0, 2) * 0.5f);
    SetFrame(path, 12, 12.0f, Vec3(1, -2, 2) * 0.5f);
    SetFrame(path, 13, 13.0f, Vec3(1, -2, 0) * 0.5f);

    m_SolverLines = new DebugDrawer();
    m_SolverPoints = new DebugDrawer();
    mTargetVisual[0] = new DebugDrawer(2);
    mTargetVisual[1] = new DebugDrawer(2);
    mTargetVisual[2] = new DebugDrawer(2);
    
} // Initialize

// ---------------------------------------------------------------------------------------------------------------------

void CCDIKApp::Update(float inDeltaTime)
{
    Application::Update(inDeltaTime);

    // Adjust [0, m_StartTime]
    m_PlayTime += inDeltaTime;
    if (m_PlayTime > m_TargetPath.GetEndTime())
    {
        m_PlayTime += m_TargetPath.GetStartTime() - m_TargetPath.GetEndTime();
    }
    
    m_Target = m_TargetPath.Sample(m_Target, m_PlayTime, true);
    m_Solver.Solve(m_Target);
    
} // Update

// ---------------------------------------------------------------------------------------------------------------------

void CCDIKApp::Render(float inAspectRatio)
{
    Application::Render(inAspectRatio);

    const float camYawRad = BasicUtils::DegToRad(m_CamYaw);
    const float camPitchRad = BasicUtils::DegToRad(m_CamPitch);
    const float sinPitch = sinf(camPitchRad);
    const Vec3 cameraPos = Vec3{cosf(camYawRad) * sinPitch, cosf(camPitchRad), sinf(camYawRad) * sinPitch} * m_CamDist;

    const Mat4 projection = Mat4::CreatePerspective(60.0f, inAspectRatio, 0.01f, 1000.0f);
    const Mat4 view = Mat4::CreateLookAt(cameraPos, {0, 0, 0}, {0, 1, 0});
    const Mat4 mvp = projection * view; // No model

    // Draw IKSolver points and lines
    m_SolverLines->LinesFromIKSolver(m_Solver);
    m_SolverPoints->PointsFromIKSolver(m_Solver);
    m_SolverLines->UpdateOpenGLBuffers();
    m_SolverPoints->UpdateOpenGLBuffers();
    m_SolverLines->Draw(DebugDrawMode::Lines, {1, 0, 1}, mvp);
    m_SolverPoints->Draw(DebugDrawMode::Points, {1, 0, 1}, mvp);

    // Draw gizmo on target
    static constexpr float GIZMO_SIZE = 0.25f;
    (*mTargetVisual[0])[0] = m_Target.position + Vec3{GIZMO_SIZE, 0, 0};
    (*mTargetVisual[1])[0] = m_Target.position + Vec3{0, GIZMO_SIZE, 0};
    (*mTargetVisual[2])[0] = m_Target.position + Vec3{0, 0, GIZMO_SIZE};
    (*mTargetVisual[0])[1] = m_Target.position - Vec3{GIZMO_SIZE, 0, 0};
    (*mTargetVisual[1])[1] = m_Target.position - Vec3{0, GIZMO_SIZE, 0};
    (*mTargetVisual[2])[1] = m_Target.position - Vec3{0, 0, GIZMO_SIZE};
    mTargetVisual[0]->UpdateOpenGLBuffers();
    mTargetVisual[1]->UpdateOpenGLBuffers();
    mTargetVisual[2]->UpdateOpenGLBuffers();
    mTargetVisual[0]->Draw(DebugDrawMode::Lines, Vec3{1, 0, 0}, mvp);
    mTargetVisual[1]->Draw(DebugDrawMode::Lines, Vec3{0, 1, 0}, mvp);
    mTargetVisual[2]->Draw(DebugDrawMode::Lines, Vec3{0, 0, 1}, mvp);
    
} // Render

// ---------------------------------------------------------------------------------------------------------------------

void CCDIKApp::Shutdown()
{
    delete m_SolverLines;
    delete m_SolverPoints;
    delete mTargetVisual[0];
    delete mTargetVisual[1];
    delete mTargetVisual[2];

    Application::Shutdown();
    
} // Shutdown

// ---------------------------------------------------------------------------------------------------------------------

void CCDIKApp::SetFrame(VectorTrack& track, int index, float time, const Vec3& value)
{
    track[index].m_Value[0] = value.x;
    track[index].m_Value[1] = value.y;
    track[index].m_Value[2] = value.z;
    track[index].m_Time = time;
    
} // SetFrame

// ---------------------------------------------------------------------------------------------------------------------
