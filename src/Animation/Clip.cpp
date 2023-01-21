#include "Animation/Clip.h"

#include <algorithm>

#include "Animation/TransformTrack.h"
#include "Core/Transform.h"
#include "SkeletalMesh/Pose.h"
#include "Utils/Utils.h"

// ---------------------------------------------------------------------------------------------------------------------

Clip::Clip() : m_Name("No name given"), m_StartTime(0.f), m_EndTime(0.f), m_Looping(true)
{
    
} // Clip

// ---------------------------------------------------------------------------------------------------------------------

Clip::~Clip()
{
    
} // ~Clip

// ---------------------------------------------------------------------------------------------------------------------

unsigned Clip::GetSize() const
{
    return m_Tracks.size();
    
} // GetSize

// ---------------------------------------------------------------------------------------------------------------------

unsigned Clip::GetIDAtIndex(unsigned idx) const
{
    return m_Tracks[idx].GetID();
    
} // GetIDAtIndex

// ---------------------------------------------------------------------------------------------------------------------

const std::string& Clip::GetName() const
{
    return m_Name;
    
} // GetName

// ---------------------------------------------------------------------------------------------------------------------

float Clip::GetStartTime() const
{
    return m_StartTime;
    
} // GetStartTime

// ---------------------------------------------------------------------------------------------------------------------

float Clip::GetEndTime() const
{
    return m_EndTime;
    
} // GetEndTime

// ---------------------------------------------------------------------------------------------------------------------

float Clip::GetDuration() const
{
    return m_EndTime - m_StartTime;
    
} // GetDuration

// ---------------------------------------------------------------------------------------------------------------------

bool Clip::IsLooping() const
{
    return m_Looping;
    
} // IsLooping

// ---------------------------------------------------------------------------------------------------------------------

void Clip::SetIDAtIndex(unsigned idx, unsigned id)
{
    return m_Tracks[idx].SetID(id);
}

// ---------------------------------------------------------------------------------------------------------------------

void Clip::SetName(const std::string& name)
{
    m_Name = name;
    
} // SetName

// ---------------------------------------------------------------------------------------------------------------------

void Clip::SetLooping(bool bLooping)
{
    m_Looping = bLooping;
    
} // SetLooping

// ---------------------------------------------------------------------------------------------------------------------

void Clip::RecalculateDuration()
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

float Clip::Sample(Pose& outPose, float t) const
{
    if (Utils::IsZero(GetDuration()))
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

TransformTrack& Clip::operator[](unsigned id)
{
    const unsigned int size = GetSize();
    for (unsigned i = 0; i < size; ++i)
    {
        if (m_Tracks[i].GetID() == id)
        {
            return m_Tracks[i];
        }
    }

    m_Tracks.emplace_back(TransformTrack());
    m_Tracks[size].SetID(id);
    return m_Tracks[size];
    
} // operator[]

// ---------------------------------------------------------------------------------------------------------------------

float Clip::AdjustTimeToFitRange(float t) const
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
        t = Utils::Clamp(t, m_StartTime, m_EndTime);
    }

    return t;
    
} // AdjustTimeToFitRange

// ---------------------------------------------------------------------------------------------------------------------
