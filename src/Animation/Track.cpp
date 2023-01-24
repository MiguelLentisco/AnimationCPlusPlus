#include "Animation/Track.h"

#include "Animation/Frame.h"
#include "Animation/Interpolation.h"
#include "Core/Quat.h"

// ---------------------------------------------------------------------------------------------------------------------

template ScalarTrack;
template VectorTrack;
template QuaternionTrack;

// ---------------------------------------------------------------------------------------------------------------------

namespace TrackHelpers
{
    inline float Interpolate(float a, float b, float t) { return a + (b - a) * t; }
    inline Vec3 Interpolate(const Vec3& a, const Vec3& b, float t) { return Vec3::Lerp(a, b, t); }
    inline Quat Interpolate(const Quat&a, const Quat& b, float t) { return Quat::NLerp(a, b.GetNeighbour(a), t); }

    inline float AdjustHermiteResult(float f) { return f; }
    inline Vec3 AdjustHermiteResult(const Vec3& v) { return v; }
    inline Quat AdjustHermiteResult(const Quat& q) { return q.Normalized(); }

    inline void Neighborhood(float a, float b) {}
    inline void Neighborhood(const Vec3& a, const Vec3& b) {}
    inline void Neighborhood(const Quat& a, Quat& b) { b = b.GetNeighbour(a); }
    
} // TrackHelpers

// ---------------------------------------------------------------------------------------------------------------------

template <typename T, unsigned N>
Track<T, N>::Track() : m_Interpolation(Interpolation::Linear)
{
    
} // Track

// ---------------------------------------------------------------------------------------------------------------------

template <typename T, unsigned N>
Track<T, N>::Track(const Track& other) : m_Frames(other.m_Frames), m_Interpolation(other.m_Interpolation)
{
    
} // Track

// ---------------------------------------------------------------------------------------------------------------------

template <typename T, unsigned N>
Track<T, N>& Track<T, N>::operator=(const Track& other)
{
    if (&other == this)
    {
        return *this;
    }
    
    m_Frames = other.m_Frames;
    m_Interpolation = other.m_Interpolation;
    return *this;
    
} // operator=

// ---------------------------------------------------------------------------------------------------------------------

template <typename T, unsigned N>
Track<T, N>::~Track()
{
    
} // ~Track

// ---------------------------------------------------------------------------------------------------------------------

template <typename T, unsigned N>
unsigned Track<T, N>::GetSize() const
{
    return m_Frames.size();
    
} // GetSize

// ---------------------------------------------------------------------------------------------------------------------

template <typename T, unsigned N>
Interpolation Track<T, N>::GetInterpolation() const
{
    return m_Interpolation;
    
} // GetInterpolation

// ---------------------------------------------------------------------------------------------------------------------

template <typename T, unsigned N>
const std::vector<Frame<N>>& Track<T, N>::GetFrames() const
{
    return m_Frames;
    
} // GetFrames

// ---------------------------------------------------------------------------------------------------------------------

template <typename T, unsigned N>
void Track<T, N>::SetInterpolation(Interpolation interpolation)
{
    m_Interpolation = interpolation;
    
} // SetInterpolation

// ---------------------------------------------------------------------------------------------------------------------

template <typename T, unsigned N>
void Track<T, N>::Resize(unsigned size)
{
    m_Frames.resize(size);
    
} // Resize

// ---------------------------------------------------------------------------------------------------------------------

template <typename T, unsigned N>
float Track<T, N>::GetStartTime() const
{
    return IsEmpty() ? 0.f : m_Frames[0].m_Time;
    
} // GetStartTime

// ---------------------------------------------------------------------------------------------------------------------

template <typename T, unsigned N>
float Track<T, N>::GetEndTime() const
{
    return IsEmpty() ? 0.f : m_Frames[GetSize() - 1].m_Time;
    
} // GetEndTime

// ---------------------------------------------------------------------------------------------------------------------

template <typename T, unsigned N>
Frame<N>& Track<T, N>::operator[](unsigned idx)
{
    return m_Frames[idx];
    
} // operator[]

// ---------------------------------------------------------------------------------------------------------------------

template <typename T, unsigned N>
const Frame<N>& Track<T, N>::operator[](unsigned idx) const
{
    return m_Frames[idx];
    
} // operator[]

// ---------------------------------------------------------------------------------------------------------------------

template <typename T, unsigned N>
bool Track<T, N>::IsEmpty() const
{
    return GetSize() == 0;
    
} // IsEmpty

// ---------------------------------------------------------------------------------------------------------------------

template <typename T, unsigned N>
bool Track<T, N>::IsValid() const
{
    return GetSize() > 1;
    
} // IsValid

// ---------------------------------------------------------------------------------------------------------------------

template <typename T, unsigned N>
T Track<T, N>::Sample(float time, bool looping) const
{
    switch (m_Interpolation)
    {
        case Interpolation::Constant:
            return SampleConstant(time, looping);
        case Interpolation::Linear:
            return SampleLinear(time, looping);
        default:
        case Interpolation::Cubic:
            return SampleCubic(time, looping);
    }
    
} // Sample

// ---------------------------------------------------------------------------------------------------------------------

template <typename T, unsigned N>
T Track<T, N>::SampleConstant(float t, bool looping) const
{
    const int currentFrame = FrameIndex(t, looping);
    if (currentFrame < 0)
    {
        return {};
    }

    return Cast(&m_Frames[currentFrame].m_Value[0]);
    
} // SampleConstant

// ---------------------------------------------------------------------------------------------------------------------

template <typename T, unsigned N>
T Track<T, N>::SampleLinear(float t, bool looping) const
{
    const int currentFrame = FrameIndex(t, looping);
    if (currentFrame < 0)
    {
        return {};
    }

    const T start = Cast(&m_Frames[currentFrame].m_Value[0]);
    if (currentFrame == GetSize() - 1)
    {
       return start;
    }
    
    const int nextFrame = currentFrame + 1;
    const float minTime = m_Frames[currentFrame].m_Time;
    const float deltaFrame = m_Frames[nextFrame].m_Time - minTime;
    if (deltaFrame <= 0.f)
    {
        return {};
    }

    t = AdjustTimeToFitTrack(t, looping);
    const float alpha = (t - minTime) / deltaFrame;
   
    const T end = Cast(&m_Frames[nextFrame].m_Value[0]);
    return TrackHelpers::Interpolate(start, end, alpha);
    
} // SampleLinear

// ---------------------------------------------------------------------------------------------------------------------

template <typename T, unsigned N>
T Track<T, N>::SampleCubic(float t, bool looping) const
{
    const int currentFrame = FrameIndex(t, looping);
    if (currentFrame < 0)
    {
        return {};
    }

    const T p1 = Cast(&m_Frames[currentFrame].m_Value[0]);
    if (currentFrame == GetSize() - 1)
    {
        return p1;
    }
    
    const int nextFrame = currentFrame + 1;
    const float minTime = m_Frames[currentFrame].m_Time;
    const float deltaFrame = m_Frames[nextFrame].m_Time - minTime;
    if (deltaFrame <= 0.f)
    {
        return {};
    }

    t = AdjustTimeToFitTrack(t, looping);
    const float alpha = (t - minTime) / deltaFrame;

    static constexpr unsigned int FLOAT_SIZE = sizeof(float);
    
    T s1;
    memcpy(&s1, m_Frames[currentFrame].m_Out, N * FLOAT_SIZE);
    s1 *= deltaFrame;
    
    const T p2 = Cast(&m_Frames[nextFrame].m_Value[0]);
    T s2;
    memcpy(&s2, m_Frames[nextFrame].m_In, N * FLOAT_SIZE);
    s2 *= deltaFrame;
    
    return Hermite(alpha, p1, s1, p2, s2);
    
} // SampleCubic

// ---------------------------------------------------------------------------------------------------------------------

template <typename T, unsigned N>
T Track<T, N>::Hermite(float t, const T& p1, const T& s1, const T& p2, const T& s2)
{
    const float tt = t * t;
    const float ttt = tt * t;
    
    const float h1 = 2.f * ttt - 3.f * tt + 1.f;
    const float h2 = -2.f * ttt + 3.f * tt;
    const float h3 = ttt - 2.f * tt + t;
    const float h4 = ttt - tt;

    T orientedP2 = p2;
    TrackHelpers::Neighborhood(p1, orientedP2);
    
    T result = p1 * h1 + orientedP2 * h2 + s1 * h3 + s2 * h4;
    return TrackHelpers::AdjustHermiteResult(result);
    
} // Hermite

// ---------------------------------------------------------------------------------------------------------------------

template <typename T, unsigned N>
int Track<T, N>::FrameIndex(float t, bool looping) const
{
    const unsigned int size = GetSize();
    if (size <= 1)
    {
        return -1;
    }
   
    const float startTime = GetStartTime();
    const float endTime = GetEndTime();
    const unsigned int lastIdx = size - 1;

    // When looping clamp to [t_start, t_end]
    if (looping)
    {
        t = fmodf(t - startTime, endTime - startTime);
        t += t >= 0.f ? startTime : endTime;
    }

    // Check limits
    if (t <= startTime)
    {
        return 0;
    }
    if (t >= m_Frames[lastIdx].m_Time)
    {
        return lastIdx;
    }
    
    // Search the frame closest to the time (but still less)
    for (unsigned int i = lastIdx - 1; i > 0; --i)
    {
        if (t >= m_Frames[i].m_Time)
        {
            return i;
        }
    }

    return 0; // Invalid
    
} // FrameIndex

// ---------------------------------------------------------------------------------------------------------------------

template <typename T, unsigned N>
float Track<T, N>::AdjustTimeToFitTrack(float t, bool looping) const
{
    if (IsEmpty())
    {
        return 0.f;
    }

    const float startTime = GetStartTime();
    const float endTime = GetEndTime();
    const float duration = endTime - startTime;
    if (duration <= 0.f)
    {
        return 0.f;
    }
    
    if (looping)
    {
        t = fmodf(t - startTime, duration);
        t += t >= 0.f ? startTime : endTime;
    }
    else if (t <= m_Frames[0].m_Time)
    {
        t = startTime;
    }
    else if (t >= m_Frames[GetSize() - 1].m_Time)
    {
        t = endTime;
    }

    return t;
    
} // AdjustTimeToFitTrack

// ---------------------------------------------------------------------------------------------------------------------

template <>
float Track<float, 1>::Cast(const float* value)
{
    return value[0];
    
} // Cast

// ---------------------------------------------------------------------------------------------------------------------

template <>
Vec3 Track<Vec3, 3>::Cast(const float* value)
{
    return {value[0], value[1], value[2]};
    
} // Cast

// ---------------------------------------------------------------------------------------------------------------------

template <>
Quat Track<Quat, 4>::Cast(const float* value)
{
   const Quat r = {value[0], value[1], value[2], value[3]};
   return r.Normalized();
    
} // Cast

// ---------------------------------------------------------------------------------------------------------------------


