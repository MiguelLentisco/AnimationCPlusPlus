#include "Animation/Clip.h"

#include <algorithm>

#include "Animation/FastTrack.h"
#include "Animation/Track.h"
#include "Animation/TransformTrack.h"
#include "Core//BasicUtils.h"
#include "Core/Transform.h"
#include "SkeletalMesh/Pose.h"

// ---------------------------------------------------------------------------------------------------------------------

template TClip<TransformTrack>;
template TClip<FastTransformTrack>;

// ---------------------------------------------------------------------------------------------------------------------

template <typename TRACK>
TClip<TRACK>::TClip() : m_Name("No name given"), m_StartTime(0.f), m_EndTime(0.f), m_Looping(true)
{
    
} // Clip

// ---------------------------------------------------------------------------------------------------------------------

template <typename TRACK>
TClip<TRACK>::~TClip()
{
    
} // ~Clip

// ---------------------------------------------------------------------------------------------------------------------

template <typename TRACK>
unsigned TClip<TRACK>::GetSize() const
{
    return m_Tracks.size();
    
} // GetSize

// ---------------------------------------------------------------------------------------------------------------------

template <typename TRACK>
unsigned TClip<TRACK>::GetIDAtIndex(unsigned idx) const
{
    return m_Tracks[idx].GetID();
    
} // GetIDAtIndex

// ---------------------------------------------------------------------------------------------------------------------

template <typename TRACK>
const std::string& TClip<TRACK>::GetName() const
{
    return m_Name;
    
} // GetName

// ---------------------------------------------------------------------------------------------------------------------

template <typename TRACK>
float TClip<TRACK>::GetStartTime() const
{
    return m_StartTime;
    
} // GetStartTime

// ---------------------------------------------------------------------------------------------------------------------

template <typename TRACK>
float TClip<TRACK>::GetEndTime() const
{
    return m_EndTime;
    
} // GetEndTime

// ---------------------------------------------------------------------------------------------------------------------

template <typename TRACK>
float TClip<TRACK>::GetDuration() const
{
    return m_EndTime - m_StartTime;
    
} // GetDuration

// ---------------------------------------------------------------------------------------------------------------------

template <typename TRACK>
bool TClip<TRACK>::IsLooping() const
{
    return m_Looping;
    
} // IsLooping

// ---------------------------------------------------------------------------------------------------------------------

template <typename TRACK>
void TClip<TRACK>::SetIDAtIndex(unsigned idx, unsigned id)
{
    return m_Tracks[idx].SetID(id);
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename TRACK>
void TClip<TRACK>::SetName(const std::string& name)
{
    m_Name = name;
    
} // SetName

// ---------------------------------------------------------------------------------------------------------------------

template <typename TRACK>
void TClip<TRACK>::SetLooping(bool bLooping)
{
    m_Looping = bLooping;
    
} // SetLooping

// ---------------------------------------------------------------------------------------------------------------------

template <typename TRACK>
void TClip<TRACK>::RecalculateDuration()
{
    m_StartTime = 0.f;
    m_EndTime = 0.f;

    bool bStartSet = false;
    bool bEndSet = false;

    const unsigned int trackSize = GetSize();
    for (unsigned int i = 0; i < trackSize; ++i)
    {
        if (!m_Tracks[i].IsValid())
        {
            continue;
        }

        const float startTime = m_Tracks[i].GetStartTime();
        const float endTime = m_Tracks[i].GetEndTime();

        if (startTime < m_StartTime || !bStartSet)
        {
            m_StartTime = startTime;
            bStartSet = true;
        }

        if (endTime > m_EndTime || !bEndSet)
        {
            m_EndTime = endTime;
            bEndSet = true;
        }
    }
    
} // RecalculateDuration

// ---------------------------------------------------------------------------------------------------------------------

template <typename TRACK>
float TClip<TRACK>::Sample(Pose& outPose, float t) const
{
    if (BasicUtils::IsZero(GetDuration()))
    {
        return 0.f;
    }

    t = AdjustTimeToFitRange(t);

    const unsigned int size = GetSize();
    for (unsigned int i = 0; i < size; ++i)
    {
        const unsigned int jointID = m_Tracks[i].GetID();
        const Transform& baseTransform = outPose.GetLocalTransform(jointID);
        const Transform animated = m_Tracks[i].Sample(baseTransform, t, m_Looping);
        outPose.SetLocalTransform(jointID, animated);
    }

    return t;
    
} // Sample

// ---------------------------------------------------------------------------------------------------------------------

template <typename TRACK>
TRACK& TClip<TRACK>::operator[](unsigned id)
{
    const unsigned int size = GetSize();
    for (unsigned i = 0; i < size; ++i)
    {
        if (m_Tracks[i].GetID() == id)
        {
            return m_Tracks[i];
        }
    }

    m_Tracks.emplace_back(TRACK());
    m_Tracks[size].SetID(id);
    return m_Tracks[size];
    
} // operator[]

// ---------------------------------------------------------------------------------------------------------------------

template <typename TRACK>
const TRACK& TClip<TRACK>::operator[](unsigned id) const
{
    const unsigned int size = GetSize();
    for (unsigned i = 0; i < size; ++i)
    {
        if (m_Tracks[i].GetID() == id)
        {
            return m_Tracks[i];
        }
    }

    m_Tracks.emplace_back(TRACK());
    m_Tracks[size].SetID(id);
    return m_Tracks[size];
    
} // operator[]

// ---------------------------------------------------------------------------------------------------------------------

template <typename TRACK>
void TClip<TRACK>::RearrangeClip(const BoneMap& boneMap)
{
    for (TRACK& track : m_Tracks)
    {
       track.SetID(static_cast<unsigned int>(boneMap.at(static_cast<int>(track.GetID()))));
    }

} // RearrangeClip

// ---------------------------------------------------------------------------------------------------------------------

template <typename TRACK>
float TClip<TRACK>::AdjustTimeToFitRange(float t) const
{
    const float duration = GetDuration();
    if (duration <= 0.f)
    {
        return 0.f;
    }
    
    if (m_Looping)
    {
        t = fmodf(t - m_StartTime, duration);
        t += t >= 0.f ? m_StartTime : m_EndTime;
    }
    else
    {
        t = BasicUtils::Clamp(t, m_StartTime, m_EndTime);
    }

    return t;
    
} // AdjustTimeToFitRange

// ---------------------------------------------------------------------------------------------------------------------