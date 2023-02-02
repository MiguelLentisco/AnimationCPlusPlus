#include "Animation/AnimationUtilities.h"

#include <algorithm>

#include "Animation/Frame.h"
#include "Animation/FastTrack.h"
#include "Animation/TransformTrack.h"
#include "Animation/Clip.h"
#include "Core/BasicUtils.h"
#include "Core/Transform.h"
#include "Render/AnimTexture.h"
#include "SkeletalMesh/Skeleton.h"

// ---------------------------------------------------------------------------------------------------------------------

template FastScalarTrack AnimationUtilities::OptimizeTrack(const ScalarTrack&);
template FastVectorTrack AnimationUtilities::OptimizeTrack(const VectorTrack&);
template FastQuaternionTrack AnimationUtilities::OptimizeTrack(const QuaternionTrack&);

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

template Pose AnimationUtilities::MakeAdditivePose(const Skeleton&, const TClip<TransformTrack>&);
template Pose AnimationUtilities::MakeAdditivePose(const Skeleton&, const TClip<FastTransformTrack>&);

template <typename TRACK>
Pose AnimationUtilities::MakeAdditivePose(const Skeleton& skeleton, const TClip<TRACK>& clip)
{
    Pose result = skeleton.GetRestPose();
    clip.Sample(result, clip.GetStartTime());
    return result;
    
} // MakeAdditivePose

// ---------------------------------------------------------------------------------------------------------------------

template void AnimationUtilities::BakeAnimationToTexture(const Skeleton&, const TClip<TransformTrack>&, AnimTexture&);
template void AnimationUtilities::BakeAnimationToTexture(const Skeleton&, const TClip<FastTransformTrack>&, AnimTexture&);

template <typename TRACK>
void AnimationUtilities::BakeAnimationToTexture(const Skeleton& skeleton, const TClip<TRACK>& clip, AnimTexture& outTex)
{
    Pose pose = skeleton.GetBindPose();
    
    const unsigned int texWidth = outTex.GetSize();
    const unsigned int numTotalChannels = pose.GetSize();
    const float start = clip.GetStartTime();
    const float end = clip.GetEndTime();
    
    for (unsigned int x = 0; x < texWidth; ++x)
    {
        const float alpha = static_cast<float>(x) / static_cast<float>(texWidth - 1);
        float time = BasicUtils::Lerp(start, end, alpha);
        clip.Sample(pose, time);
        
        for (unsigned int y = 0; y < numTotalChannels; ++y)
        {
            const unsigned int yShift = 3 * y;
            Transform jointTransform = pose.GetGlobalTransform(y);
            outTex.SetTexel(x, yShift + 0, jointTransform.position);
            outTex.SetTexel(x, yShift + 1, jointTransform.rotation);
            outTex.SetTexel(x, yShift + 2, jointTransform.scale);
        }
    }

    outTex.UploadTextureDataToGPU();
    
} // BakeAnimationToTexture

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

