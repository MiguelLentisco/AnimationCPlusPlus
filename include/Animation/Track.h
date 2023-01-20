#pragma once

#include <vector>

template <unsigned int N> class Frame;
typedef Frame<1> ScalarFrame;
typedef Frame<3> VectorFrame;
typedef Frame<4> QuaternionFrame;

enum class Interpolation;
struct Vec3;
struct Quat;

template<typename T, unsigned int N>
class Track
{
public:
    Track();
    Track(const Track& other);
    Track& operator=(const Track& other);
    ~Track();
    
    unsigned int GetSize() const;
    Interpolation GetInterpolation() const;
    
    void SetInterpolation(Interpolation interpolation);
    void Resize(unsigned int size);

    Frame<N>& operator[](unsigned int idx);
    const Frame<N>& operator[](unsigned int idx) const;

    bool IsEmpty() const;
    bool IsValid() const;
    
    float GetStartTime() const;
    float GetEndTime() const;

    T Sample(float time, bool looping) const;
    
protected:
    std::vector<Frame<N>> m_Frames;
    Interpolation m_Interpolation;

    T SampleConstant(float t, bool looping) const;
    T SampleLinear(float t, bool looping) const;
    T SampleCubic(float t, bool looping) const;
    static T Hermite(float t, const T& p1, const T& s1, const T& p2, const T& s2);

    int FrameIndex(float t, bool looping) const;
    float AdjustTimeToFitTrack(float t, bool looping) const;

    static T Cast(const float* value); // Will be specialized
    
}; // Track

typedef Track<float, 1> ScalarTrack;
typedef Track<Vec3, 3> VectorTrack;
typedef Track<Quat, 4> QuaternionTrack;