#include "Animation/Crowd.h"

#include <random>

#include "Animation/Clip.h"
#include "Core/BasicUtils.h"
#include "Core/Transform.h"
#include "Core/TVec2.h"
#include "Render/Shader.h"
#include "Render/Uniform.h"

// ---------------------------------------------------------------------------------------------------------------------

unsigned Crowd::GetSize() const
{
    return m_CurrentPlayTimes.size();
    
} // GetSize

// ---------------------------------------------------------------------------------------------------------------------

Transform Crowd::GetActor(unsigned idx) const
{
    return {m_Positions[idx], m_Rotations[idx], m_Scales[idx]};
    
} // GetActor

// ---------------------------------------------------------------------------------------------------------------------

void Crowd::Resize(unsigned size)
{
    static constexpr unsigned int CROWD_MAX_ACTORS = 80;
    size = std::min(size, CROWD_MAX_ACTORS);

    m_Positions.resize(size);
    m_Rotations.resize(size);
    m_Scales.resize(size, Vec3{1, 1, 1});
    m_Frames.resize(size);
    m_Times.resize(size);
    m_CurrentPlayTimes.resize(size);
    m_NextPlayTimes.resize(size);
    
} // Resize

// ---------------------------------------------------------------------------------------------------------------------

void Crowd::SetActor(unsigned idx, const Transform& t)
{
    m_Positions[idx] = t.position;
    m_Rotations[idx] = t.rotation;
    m_Scales[idx] = t.scale;
    
} // SetActor

// ---------------------------------------------------------------------------------------------------------------------

void Crowd::SetUniforms(const Shader* shader) const
{
    Uniform<Vec3>::Set(shader->GetUniform("model_pos"), m_Positions);   
    Uniform<Quat>::Set(shader->GetUniform("model_rot"), m_Rotations);   
    Uniform<Vec3>::Set(shader->GetUniform("model_scl"), m_Scales);   
    Uniform<IVec2>::Set(shader->GetUniform("frames"), m_Frames);
    Uniform<float>::Set(shader->GetUniform("time"), m_Times);
    
} // SetUniforms

// ---------------------------------------------------------------------------------------------------------------------

template void Crowd::Update(float, const FastClip&, unsigned);
template void Crowd::Update(float, const Clip&, unsigned);

template <typename TRACK>
void Crowd::Update(float deltaTime, const TClip<TRACK>& clip, unsigned texWidth)
{
    const bool bLooping = clip.IsLooping();
    const float start = clip.GetStartTime();
    const float end = clip.GetEndTime();
    const float duration = end - start;

    UpdatePlaybackTimes(deltaTime, bLooping, start, end);
    UpdateFrameIndices(start, duration, texWidth);
    UpdateInterpolationTimes(start, duration, texWidth);
    
} // Update

// ---------------------------------------------------------------------------------------------------------------------

template void Crowd::RandomizeTimes(const Clip&);
template void Crowd::RandomizeTimes(const FastClip&);

template <typename TRACK>
void Crowd::RandomizeTimes(const TClip<TRACK>& clip)
{
    // https://en.cppreference.com/w/cpp/numeric/random/uniform_int_distribution
    static std::random_device rd;  //Will be used to obtain a seed for the random number engine
    static std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    const std::uniform_real_distribution<float> uniformDist(clip.GetStartTime(), clip.GetEndTime());
    
    for (float& currentTime : m_CurrentPlayTimes)
    {
        currentTime = uniformDist(gen);
    }
    
} // RandomizeTimes

// ---------------------------------------------------------------------------------------------------------------------

void Crowd::RandomizePositions(const Vec3& min, const Vec3& max, float radius)
{
    const unsigned int size = m_CurrentPlayTimes.size();
    unsigned int breakLoop = 0;
    unsigned int currentSize = 0;
    
    while (currentSize < size)
    {
        static constexpr unsigned int MAX_LOOP = 2000;
        if (breakLoop >= MAX_LOOP)
        {
            return;
        }

        static std::random_device rd;  //Will be used to obtain a seed for the random number engine
        static std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
        const std::uniform_real_distribution<float> uniformDistX(min.x, max.x);
        const std::uniform_real_distribution<float> uniformDistY(min.y, max.y);
        const std::uniform_real_distribution<float> uniformDistZ(min.z, max.z);
        
        const Vec3 newPoint = {uniformDistX(gen), uniformDistY(gen), uniformDistZ(gen)};
        const float radiusSq = radius * radius;
        bool bValid = true;
        
        for (const Vec3& pos : m_Positions)
        {
            if ((pos - newPoint).LenSq() < radiusSq)
            {
                bValid = false;
                breakLoop += 1;
                break;
            }
        }

        if (!bValid)
        {
            continue;
        }

        m_Positions[currentSize] = newPoint;
        ++currentSize;
    }
    
} // RandomizePositions

// ---------------------------------------------------------------------------------------------------------------------

float Crowd::AdjustTime(float t, float start, float end, bool bLooping)
{
    const float duration = end - start;
    if (duration <= 0.f)
    {
        return 0.f;
    }
    
    if (bLooping)
    {
        t = fmodf(t - start, duration);
        t += t >= 0.f ? start : end;
    }
    else
    {
        t = BasicUtils::Clamp(t, start, end);
    }

    return t;
    
} // AdjustTime

// ---------------------------------------------------------------------------------------------------------------------

void Crowd::UpdatePlaybackTimes(float dt, bool bLooping, float start, float end)
{
    const unsigned int size = m_CurrentPlayTimes.size();
    for (unsigned int i = 0; i < size; ++i)
    {
        m_CurrentPlayTimes[i] = AdjustTime(m_CurrentPlayTimes[i] + dt, start, end, bLooping);
        m_NextPlayTimes[i] = AdjustTime(m_CurrentPlayTimes[i] + dt, start, end, bLooping);
    }
    
} // UpdatePlaybackTimes

// ---------------------------------------------------------------------------------------------------------------------

void Crowd::UpdateFrameIndices(float start, float duration, unsigned textWidth)
{
    const auto texSize = static_cast<float>(textWidth - 1);
    const unsigned int size = m_CurrentPlayTimes.size();
    
    for (unsigned int i = 0; i < size; ++i)
    {
        const float normCurrentTime = (m_CurrentPlayTimes[i] - start) / duration;
        const float normNextTime = (m_NextPlayTimes[i] - start) / duration;
        m_Frames[i].x = static_cast<int>(normCurrentTime * texSize);
        m_Frames[i].y = static_cast<int>(normNextTime * texSize);
    }
    
} // UpdateFrameIndices

// ---------------------------------------------------------------------------------------------------------------------

void Crowd::UpdateInterpolationTimes(float start, float duration, unsigned textWidth)
{
    const auto texSize = static_cast<float>(textWidth - 1);
    const unsigned int size = m_CurrentPlayTimes.size();
    
    for (unsigned int i = 0; i < size; ++i)
    {
        if (m_Frames[i].x == m_Frames[i].y)
        {
            m_Times[i] = 1;
            continue;
        }

        const float currentFrameTime = start + duration * (static_cast<float>(m_Frames[i].x) / texSize);
        float nextFrameTime = start + duration * (static_cast<float>(m_Frames[i].y) / texSize);
        if (nextFrameTime < currentFrameTime)
        {
            nextFrameTime += duration;
        }

        m_Times[i] = (m_CurrentPlayTimes[i] - currentFrameTime) / (nextFrameTime - currentFrameTime);
    }
    
} // UpdateInterpolationTimes

// ---------------------------------------------------------------------------------------------------------------------

