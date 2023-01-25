#include "Animation/Blend/CrossFadeController.h"

#include "Animation/Clip.h"
#include "Animation/FastTrack.h"
#include "Animation/TransformTrack.h"
#include "Animation/Blend/CrossFadeTarget.h"
#include "Core/Mat4.h"

// ---------------------------------------------------------------------------------------------------------------------

template CrossFadeController<TransformTrack>;
template CrossFadeController<FastTransformTrack>;

// ---------------------------------------------------------------------------------------------------------------------

template <typename TRACK>
CrossFadeController<TRACK>::CrossFadeController()
{
    
} // CrossFadeController

// ---------------------------------------------------------------------------------------------------------------------

template <typename TRACK>
CrossFadeController<TRACK>::CrossFadeController(const Skeleton& skeleton) : m_Skeleton(skeleton),
    m_Pose(skeleton.GetRestPose()), m_WasSkeletonSet(true)
{
    
} // CrossFadeController

// ---------------------------------------------------------------------------------------------------------------------

template <typename TRACK>
void CrossFadeController<TRACK>::SetSkeleton(const Skeleton& skeleton)
{
    m_Skeleton = skeleton;
    m_Pose = m_Skeleton.GetRestPose();
    m_WasSkeletonSet = true;
    
} // SetSkeleton

// ---------------------------------------------------------------------------------------------------------------------

template <typename TRACK>
void CrossFadeController<TRACK>::Play(TClip<TRACK>* target)
{
    m_Targets.clear();
    m_CurrentClip = target;
    m_Pose = m_Skeleton.GetRestPose();
    m_Time = target->GetStartTime();
    
} // Play

// ---------------------------------------------------------------------------------------------------------------------

template <typename TRACK>
void CrossFadeController<TRACK>::FadeTo(TClip<TRACK>* target, float fadeTime)
{
    if (m_CurrentClip == nullptr)
    {
        Play(target);
        return;
    }

    if (m_Targets.size() >= 1 && m_Targets[m_Targets.size() - 1].m_Clip == target)
    {
        return;
    }
    if (m_CurrentClip == target)
    {
        return;
    }

    m_Targets.push_back(TCrossFadeTarget<TRACK>(target, m_Skeleton.GetRestPose(), fadeTime));
    
} // FadeTo

// ---------------------------------------------------------------------------------------------------------------------

template <typename TRACK>
void CrossFadeController<TRACK>::Update(float deltaTime)
{
    if (m_CurrentClip == nullptr || !m_WasSkeletonSet)
    {
        return;
    }

    deltaTime *= m_PlaybackTime;
    
    // Remove finished blends, and set the last one
    for (unsigned int i = 0; i < m_Targets.size(); ++i)
    {
        TCrossFadeTarget<TRACK>& target = m_Targets[i];
        target.m_Elapsed += deltaTime;
        
        if (target.m_Elapsed < target.m_Duration)
        {
            continue;
        }

        m_CurrentClip = target.m_Clip;
        m_Time = target.m_Time;
        m_Targets.erase(m_Targets.begin() + i);
        --i;
    }

    m_Pose = m_Skeleton.GetRestPose();
    m_Time = m_CurrentClip->Sample(m_Pose, m_Time + deltaTime);

    for (TCrossFadeTarget<TRACK>& target : m_Targets)
    {
        target.m_Time = target.m_Clip->Sample(target.m_Pose, target.m_Time + deltaTime);
        const float alpha = target.m_Elapsed / target.m_Duration;
        static constexpr int ROOT_BONE = -1;
        Pose::Blend(m_Pose, m_Pose, target.m_Pose, alpha, ROOT_BONE);
    }
    
} // Update

// ---------------------------------------------------------------------------------------------------------------------