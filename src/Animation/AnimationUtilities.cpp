#include "Animation/AnimationUtilities.h"

#include <algorithm>

#include "Animation/Frame.h"
#include "Animation/FastTrack.h"
#include "Animation/TransformTrack.h"
#include "Animation/Clip.h"

// ---------------------------------------------------------------------------------------------------------------------

template <typename T, unsigned N>
FastTrack<T, N> AnimationUtilities::OptimizeTrack(const Track<T, N>& track)
{
    FastTrack<T, N> result;
    
    result.SetInterpolation(track.GetInterpolation());
    
    unsigned int size = track.GetSize();
    result.Resize(size);

    std::copy(track.GetFrames().begin(), track.GetFrames().end(), result.GetFrames().begin());
    result.UpdateIndexLookupTable();

    return result;
    
} // OptimizeTrack

// ---------------------------------------------------------------------------------------------------------------------

FastTransformTrack AnimationUtilities::OptimizeTransformTrack(const TransformTrack& transformTrack)
{
    FastTransformTrack result;

    result.SetID(transformTrack.GetID());
    result.GetPositionTrack() = OptimizeTrack(transformTrack.GetPositionTrack());
    result.GetRotationTrack() = OptimizeTrack(transformTrack.GetRotationTrack());
    result.GetScaleTrack() = OptimizeTrack(transformTrack.GetScaleTrack());

    return result;
    
} // OptimizeTransformTrack

// ---------------------------------------------------------------------------------------------------------------------

FastClip AnimationUtilities::OptimizeClip(const Clip& clip)
{
    FastClip result;

    result.SetName(clip.GetName());
    result.SetLooping(clip.IsLooping());

    const unsigned int size = clip.GetSize();
    for (unsigned int i = 0; i < size; ++i)
    {
        const unsigned boneID = clip.GetIDAtIndex(i);
        result[boneID] = OptimizeTransformTrack(clip[boneID]);
    }

    result.RecalculateDuration();
    return result;
    
} // OptimizeClip

// ---------------------------------------------------------------------------------------------------------------------

