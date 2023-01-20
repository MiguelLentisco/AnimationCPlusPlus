#include "Animation/TransformTrack.h"

#include "Core/Transform.h"

// ---------------------------------------------------------------------------------------------------------------------

TransformTrack::TransformTrack() : m_ID(0)
{
    
} // TransformTrack

// ---------------------------------------------------------------------------------------------------------------------

unsigned TransformTrack::GetID() const
{
    return m_ID;
    
} // GetID

// ---------------------------------------------------------------------------------------------------------------------

VectorTrack& TransformTrack::GetPositionTrack()
{
    return m_Position;
    
} // GetPositionTrack

// ---------------------------------------------------------------------------------------------------------------------

const VectorTrack& TransformTrack::GetPositionTrack() const
{
    return m_Position;
    
} // GetPositionTrack

// ---------------------------------------------------------------------------------------------------------------------

QuaternionTrack& TransformTrack::GetRotationTrack()
{
    return m_Rotation;
    
} // GetRotationTrack

// ---------------------------------------------------------------------------------------------------------------------

const QuaternionTrack& TransformTrack::GetRotationTrack() const
{
    return m_Rotation;
    
} // GetRotationTrack

// ---------------------------------------------------------------------------------------------------------------------

VectorTrack& TransformTrack::GetScaleTrack()
{
    return m_Scale;
    
} // GetScaleTrack

// ---------------------------------------------------------------------------------------------------------------------

const VectorTrack& TransformTrack::GetScaleTrack() const
{
    return m_Scale;
    
} // GetScaleTrack

// ---------------------------------------------------------------------------------------------------------------------

void TransformTrack::SetID(unsigned id)
{
    m_ID = id;
    
} // SetID

// ---------------------------------------------------------------------------------------------------------------------

float TransformTrack::GetStartTime() const
{
    float startTime = 0.f;
    bool bSet = false;

    CheckStartTime(m_Position, bSet, startTime);
    CheckStartTime(m_Rotation, bSet, startTime);
    CheckStartTime(m_Scale, bSet, startTime);

    return startTime;
    
} // GetStartTime

// ---------------------------------------------------------------------------------------------------------------------

float TransformTrack::GetEndTime() const
{
    float endTime = 0.f;
    bool bSet = false;

    CheckEndTime(m_Position, bSet, endTime);
    CheckEndTime(m_Rotation, bSet, endTime);
    CheckEndTime(m_Scale, bSet, endTime);

    return endTime;
    
} // GetEndTime

// ---------------------------------------------------------------------------------------------------------------------

bool TransformTrack::IsValid() const
{
    return m_Position.IsValid() || m_Rotation.IsValid() || m_Scale.IsValid();
    
} // IsValid

// ---------------------------------------------------------------------------------------------------------------------

Transform TransformTrack::Sample(const Transform& ref, float t, bool looping) const
{
    Transform result = ref;

    if (m_Position.IsValid())
    {
        result.position = m_Position.Sample(t, looping);
    }

    if (m_Rotation.IsValid())
    {
        result.rotation = m_Rotation.Sample(t, looping);
    }

    if (m_Scale.IsValid())
    {
        result.scale = m_Scale.Sample(t, looping);
    }

    return result;
    
} // Sample

// ---------------------------------------------------------------------------------------------------------------------

template <typename T, unsigned N>
void TransformTrack::CheckStartTime(const Track<T, N>& track, bool& bSet, float& startTime)
{
    if (!track.IsValid())
    {
        return;
    }

    const float trackStartTime = track.GetStartTime();
    if (!bSet || trackStartTime < startTime)
    {
        startTime = trackStartTime;
        bSet = true;
    }
    
} // CheckStartTime

// ---------------------------------------------------------------------------------------------------------------------

template <typename T, unsigned N>
void TransformTrack::CheckEndTime(const Track<T, N>& track, bool& bSet, float& endTime)
{
    if (!track.IsValid())
    {
        return;
    }

    const float trackEndTime = track.GetEndTime();
    if (!bSet || trackEndTime > endTime)
    {
        endTime = trackEndTime;
        bSet = true;
    }
    
} // CheckEndTime

// ---------------------------------------------------------------------------------------------------------------------
