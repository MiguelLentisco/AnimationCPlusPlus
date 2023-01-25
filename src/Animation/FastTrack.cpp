#include "Animation/FastTrack.h"

#include "Animation/Frame.h"
#include "Core/BasicUtils.h"

// ---------------------------------------------------------------------------------------------------------------------

template FastScalarTrack;
template FastVectorTrack;
template FastQuaternionTrack;

// ---------------------------------------------------------------------------------------------------------------------

template <typename T, unsigned N>
void FastTrack<T, N>::UpdateIndexLookupTable()
{
    const unsigned int numFrames = this->GetSize();
    if (numFrames <= 1)
    {
        return;
    }

    const float startTime = this->GetStartTime();
    const float endTime = this->GetEndTime();
    const float duration = endTime - startTime;
    const unsigned int numSamples = static_cast<unsigned int>(duration * FPS);

    m_SampledFrames.resize(numSamples);
    m_SampledFrames[0] = 0;
    m_SampledFrames[numSamples - 1] = numFrames - 1; // Special case
    
    for (unsigned int i = 1; i < numSamples - 1; ++i)
    {
        const float alpha = static_cast<float>(i) / static_cast<float>(numSamples - 1);
        const float t = BasicUtils::Lerp(startTime, endTime, alpha);
        
        // Search index
        unsigned int frameIdx = 0;
        for (unsigned int j = numFrames - 2; j > 0; --j)
        {
            if (t < this->m_Frames[j].m_Time)
            {
                continue;
            }

            frameIdx = j;
            break;
        }
        
        m_SampledFrames[i] = frameIdx;
    }
    
} // UpdateIndexLookupTable

// ---------------------------------------------------------------------------------------------------------------------

template <typename T, unsigned N>
int FastTrack<T, N>::FrameIndex(float t, bool bLooping) const
{
    const unsigned int size = this->GetSize();
    if (size <= 1)
    {
        return -1;
    }
   
    const float startTime = this->GetStartTime();
    const float endTime = this->GetEndTime();
    const float duration = endTime - startTime;
    const unsigned int lastIdx = size - 1;

    // When looping clamp to [t_start, t_end]
    if (bLooping)
    {
        t = fmodf(t - startTime, duration);
        t += t >= 0.f ? startTime : endTime;
    }

    // Check limits
    if (t <= startTime)
    {
        return 0;
    }
    if (t >= this->m_Frames[lastIdx].m_Time)
    {
        return lastIdx;
    }
    
    // Lookup table
    const float alpha = (t - startTime) / duration;
    const unsigned int numSamples = static_cast<unsigned int>(duration * FPS);
    const unsigned int idx = static_cast<unsigned int>(alpha * static_cast<float>(numSamples) - 1);
    
    return static_cast<int>(m_SampledFrames[idx]);
    
} // FrameIndex

// ---------------------------------------------------------------------------------------------------------------------

