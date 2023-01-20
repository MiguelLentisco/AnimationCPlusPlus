#pragma once

#include "Track.h"

struct Transform;

class TransformTrack
{
public:
    TransformTrack();

    unsigned int GetID() const;
    VectorTrack& GetPositionTrack();
    const VectorTrack& GetPositionTrack() const;
    QuaternionTrack& GetRotationTrack();
    const QuaternionTrack& GetRotationTrack() const;
    VectorTrack& GetScaleTrack();
    const VectorTrack& GetScaleTrack() const;

    void SetID(unsigned int id);

    float GetStartTime() const;
    float GetEndTime() const;

    bool IsValid() const;

    Transform Sample(const Transform& ref, float t, bool looping) const;

protected:
    unsigned int m_ID; // Bone ID
    VectorTrack m_Position;
    QuaternionTrack m_Rotation;
    VectorTrack m_Scale;

private:
    template <typename T, unsigned int N>
    static void CheckStartTime(const Track<T, N>& track, bool& bSet, float& startTime);
    template <typename T, unsigned int N>
    static void CheckEndTime(const Track<T, N>& track, bool& bSet, float& endTime);
    
}; // TransformTrack
