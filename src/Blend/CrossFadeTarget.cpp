#include "Blend/CrossFadeTarget.h"

#include "Animation/Clip.h"
#include "Animation/TransformTrack.h"

// ---------------------------------------------------------------------------------------------------------------------

template TCrossFadeTarget<TransformTrack>;
template TCrossFadeTarget<FastTransformTrack>;

// ---------------------------------------------------------------------------------------------------------------------

template <typename TRACK>
TCrossFadeTarget<TRACK>::TCrossFadeTarget(TClip<TRACK>* clip, const Pose& pose, float duration) : m_Pose(pose),
    m_Clip(clip), m_Time(clip->GetStartTime()), m_Duration(duration)
{
    
} // TCrossFadeTarget

// ---------------------------------------------------------------------------------------------------------------------
