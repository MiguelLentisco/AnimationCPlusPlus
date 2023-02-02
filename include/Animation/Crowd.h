#pragma once

#include <vector>

struct Vec3;
struct Quat;
struct Transform;
template<typename T> struct TVec2;
typedef TVec2<int> IVec2;
template<typename T> class TClip;
class Shader;

class Crowd
{
public:
    unsigned int GetSize() const;
    Transform GetActor(unsigned int idx) const;

    void Resize(unsigned int size);
    void SetActor(unsigned int idx, const Transform& t);
    void SetUniforms(const Shader* shader) const;

    template <typename TRACK>
    void Update(float deltaTime, const TClip<TRACK>& clip, unsigned int texWidth);
    
protected:
    std::vector<Vec3> m_Positions;
    std::vector<Quat> m_Rotations;
    std::vector<Vec3> m_Scales;
    std::vector<IVec2> m_Frames;
    std::vector<float> m_Times;
    std::vector<float> m_CurrentPlayTimes;
    std::vector<float> m_NextPlayTimes;

    static float AdjustTime(float t, float start, float end, bool bLooping);
    void UpdatePlaybackTimes(float dt, bool bLooping, float start, float end);
    void UpdateFrameIndices(float start, float duration, unsigned int textWidth);
    void UpdateInterpolationTimes(float start, float duration, unsigned int textWidth);
    
}; // Crowd
