#pragma once

#include <vector>

#include "Application.h"
#include "Animation/Track.h"

struct Quat;
struct Vec3;
class DebugDrawer;

class InterpolationsApp : public Application
{
public:
    void Initialize() override;
    void Render(float inAspectRatio) override;
    void Shutdown() override;

private:
    std::vector<ScalarTrack> m_ScalarTracks;
    std::vector<bool> m_ScalarTracksLooping;

    DebugDrawer* m_ScalarTrackLines = nullptr;
    DebugDrawer* m_HandleLines = nullptr;
    DebugDrawer* m_HandlePoints = nullptr;
    DebugDrawer* m_ReferenceLines = nullptr;

    static ScalarFrame MakeFrame(float time, float value);
    static ScalarFrame MakeFrame(float time, float in, float value, float out);
    static VectorFrame MakeFrame(float time, const Vec3& value);
    static VectorFrame MakeFrame(float time, const Vec3& in, const Vec3& value, const Vec3& out);
    static QuaternionFrame MakeFrame(float time, const Quat& value);
    static QuaternionFrame MakeFrame(float time, const Quat& in, const Quat& out, const Quat& value);
    static ScalarTrack MakeScalarTrack(Interpolation interp, int numFrames, ...);
    static VectorTrack MakeVectorTrack(Interpolation interp, int numFrames, ...);
    static QuaternionTrack MakeQuaternionTrack(Interpolation interp, int numFrames, ...);

}; // InterpolationsApp
