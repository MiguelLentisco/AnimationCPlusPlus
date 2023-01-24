#include "Animation/TransformTrack.h"

#include "Animation/FastTrack.h"
#include "Core/Transform.h"

// ---------------------------------------------------------------------------------------------------------------------

template TransformTrack;
template FastTransformTrack;

// ---------------------------------------------------------------------------------------------------------------------

template <typename VTRACK, typename QTRACK>
TTransformTrack<VTRACK, QTRACK>::TTransformTrack() : m_ID(0)
{
    
} // TTransformTrack

// ---------------------------------------------------------------------------------------------------------------------

template <typename VTRACK, typename QTRACK>
TTransformTrack<VTRACK, QTRACK>::TTransformTrack(const TTransformTrack& other) : m_ID(other.m_ID),
    m_Position(other.m_Position), m_Rotation(other.m_Rotation), m_Scale(other.m_Scale)
{
    
} // TTransformTrack

// ---------------------------------------------------------------------------------------------------------------------

template <typename VTRACK, typename QTRACK>
TTransformTrack<VTRACK, QTRACK>& TTransformTrack<VTRACK, QTRACK>::operator=(const TTransformTrack& other)
{
    if (this == &other)
    {
        return *this;
    }

    m_ID = other.m_ID;
    m_Position = other.m_Position;
    m_Rotation = other.m_Rotation;
    m_Scale = other.m_Scale;

    return *this;
    
} // operator=

// ---------------------------------------------------------------------------------------------------------------------

template <typename VTRACK, typename QTRACK>
TTransformTrack<VTRACK, QTRACK>::~TTransformTrack()
{
    
} // ~TTransformTrack

// ---------------------------------------------------------------------------------------------------------------------

template <typename VTRACK, typename QTRACK>
unsigned TTransformTrack<VTRACK, QTRACK>::GetID() const
{
    return m_ID;
    
} // GetID

// ---------------------------------------------------------------------------------------------------------------------

template <typename VTRACK, typename QTRACK>
VTRACK& TTransformTrack<VTRACK, QTRACK>::GetPositionTrack()
{
    return m_Position;
    
} // GetPositionTrack

// ---------------------------------------------------------------------------------------------------------------------

template <typename VTRACK, typename QTRACK>
const VTRACK& TTransformTrack<VTRACK, QTRACK>::GetPositionTrack() const
{
    return m_Position;
    
} // GetPositionTrack

// ---------------------------------------------------------------------------------------------------------------------

template <typename VTRACK, typename QTRACK>
QTRACK& TTransformTrack<VTRACK, QTRACK>::GetRotationTrack()
{
    return m_Rotation;
    
} // GetRotationTrack

// ---------------------------------------------------------------------------------------------------------------------

template <typename VTRACK, typename QTRACK>
const QTRACK& TTransformTrack<VTRACK, QTRACK>::GetRotationTrack() const
{
    return m_Rotation;
    
} // GetRotationTrack

// ---------------------------------------------------------------------------------------------------------------------

template <typename VTRACK, typename QTRACK>
VTRACK& TTransformTrack<VTRACK, QTRACK>::GetScaleTrack()
{
    return m_Scale;
    
} // GetScaleTrack

// ---------------------------------------------------------------------------------------------------------------------

template <typename VTRACK, typename QTRACK>
const VTRACK& TTransformTrack<VTRACK, QTRACK>::GetScaleTrack() const
{
    return m_Scale;
    
} // GetScaleTrack

// ---------------------------------------------------------------------------------------------------------------------

template <typename VTRACK, typename QTRACK>
void TTransformTrack<VTRACK, QTRACK>::SetID(unsigned id)
{
    m_ID = id;
    
} // SetID

// ---------------------------------------------------------------------------------------------------------------------

template <typename VTRACK, typename QTRACK>
float TTransformTrack<VTRACK, QTRACK>::GetStartTime() const
{
    float startTime = 0.f;
    bool bSet = false;

    CheckStartTime(m_Position, bSet, startTime);
    CheckStartTime(m_Rotation, bSet, startTime);
    CheckStartTime(m_Scale, bSet, startTime);

    return startTime;
    
} // GetStartTime

// ---------------------------------------------------------------------------------------------------------------------

template <typename VTRACK, typename QTRACK>
float TTransformTrack<VTRACK, QTRACK>::GetEndTime() const
{
    float endTime = 0.f;
    bool bSet = false;

    CheckEndTime(m_Position, bSet, endTime);
    CheckEndTime(m_Rotation, bSet, endTime);
    CheckEndTime(m_Scale, bSet, endTime);

    return endTime;
    
} // GetEndTime

// ---------------------------------------------------------------------------------------------------------------------

template <typename VTRACK, typename QTRACK>
bool TTransformTrack<VTRACK, QTRACK>::IsValid() const
{
    return m_Position.IsValid() || m_Rotation.IsValid() || m_Scale.IsValid();
    
} // IsValid

// ---------------------------------------------------------------------------------------------------------------------

template <typename VTRACK, typename QTRACK>
Transform TTransformTrack<VTRACK, QTRACK>::Sample(const Transform& ref, float t, bool looping) const
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

template <typename VTRACK, typename QTRACK>
template<typename T, unsigned N>
void TTransformTrack<VTRACK, QTRACK>::CheckStartTime(const Track<T, N>& track, bool& bSet, float& startTime)
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

template <typename VTRACK, typename QTRACK>
template<typename T, unsigned N>
void TTransformTrack<VTRACK, QTRACK>::CheckEndTime(const Track<T, N>& track, bool& bSet, float& endTime)
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

FastTransformTrack TransformUtilities::OptimizeTransformTrack(const TransformTrack& transformTrack)
{
    FastTransformTrack result;

    result.SetID(transformTrack.GetID());
    result.GetPositionTrack() = FastVectorTrack::OptimizeTrack(transformTrack.GetPositionTrack());
    result.GetRotationTrack() = FastQuaternionTrack::OptimizeTrack(transformTrack.GetRotationTrack());
    result.GetScaleTrack() = FastVectorTrack::OptimizeTrack(transformTrack.GetScaleTrack());

    return result;
    
} // OptimizeTransformTrack

// ---------------------------------------------------------------------------------------------------------------------
