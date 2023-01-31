#pragma once

#include <vector>

class FABRIKSolver;
class CCDSolver;
class Pose;
class Shader;
struct Vec3;
struct Mat4;
template <typename T> class Attribute;

enum class DebugDrawMode
{
    Lines,
    Loop,
    Strip,
    Points,
    
}; // DebugDrawMode

class DebugDrawer
{
public:
    DebugDrawer(unsigned int size = 0);
    ~DebugDrawer();
    
    DebugDrawer(const DebugDrawer&) = delete;
    DebugDrawer& operator=(const DebugDrawer&) = delete;

    Vec3& operator[](unsigned int index);
    const Vec3& operator[](unsigned int index) const;

    void Resize(unsigned int size);
    void Push(const Vec3& v);
    void UpdateOpenGLBuffers();
    void Draw(DebugDrawMode mode, const Vec3& color, const Mat4& mvp);

    void FromPose(const Pose& pose);
    void LinesFromIKSolver(const CCDSolver& solver);
    void PointsFromIKSolver(const CCDSolver& solver);
    void LinesFromIKSolver(const FABRIKSolver& solver);
    void PointsFromIKSolver(const FABRIKSolver& solver);

protected:
    Shader* m_Shader;
    std::vector<Vec3> m_Points;
    Attribute<Vec3>* m_Attributes;
    
}; // DebugDrawer
