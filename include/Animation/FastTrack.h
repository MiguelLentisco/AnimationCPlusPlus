#pragma once

#include "Track.h"

template <typename T, unsigned int N>
class FastTrack : public Track<T, N>
{
public:
    void UpdateIndexLookupTable();

    static FastTrack<T, N> OptimizeTrack(const Track<T, N>& track);

protected:
    static constexpr float FPS = 60.f;
    
    std::vector<unsigned int> m_SampledFrames;

    int FrameIndex(float t, bool bLooping) const override;
    
}; // FastTrack

typedef FastTrack<float, 1> FastScalarTrack;
typedef FastTrack<Vec3, 3> FastVectorTrack;
typedef FastTrack<Quat, 4> FastQuaternionTrack;