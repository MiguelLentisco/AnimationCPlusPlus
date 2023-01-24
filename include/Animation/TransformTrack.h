#pragma once

struct Vec3;
struct Quat;
struct Transform;

template<typename T, unsigned int N> class Track;
template <typename T, unsigned int N> class FastTrack;

template <typename VTRACK, typename QTRACK>
class TTransformTrack
{
public:
    TTransformTrack();
    TTransformTrack(const TTransformTrack& other);
    TTransformTrack& operator=(const TTransformTrack& other);
    ~TTransformTrack();

    unsigned int GetID() const;
    VTRACK& GetPositionTrack();
    const VTRACK& GetPositionTrack() const;
    QTRACK& GetRotationTrack();
    const QTRACK& GetRotationTrack() const;
    VTRACK& GetScaleTrack();
    const VTRACK& GetScaleTrack() const;

    void SetID(unsigned int id);

    float GetStartTime() const;
    float GetEndTime() const;

    bool IsValid() const;

    Transform Sample(const Transform& ref, float t, bool looping) const;

protected:
    unsigned int m_ID; // Bone ID
    VTRACK m_Position;
    QTRACK m_Rotation;
    VTRACK m_Scale;

private:
    template <typename T, unsigned int N>
    static void CheckStartTime(const Track<T, N>& track, bool& bSet, float& startTime);
    template <typename T, unsigned int N>
    static void CheckEndTime(const Track<T, N>& track, bool& bSet, float& endTime);
    
}; // TransformTrack

typedef TTransformTrack<Track<Vec3, 3>, Track<Quat, 4>> TransformTrack;
typedef TTransformTrack<FastTrack<Vec3, 3>, FastTrack<Quat, 4>> FastTransformTrack;

class TransformUtilities
{
public:
    static FastTransformTrack OptimizeTransformTrack(const TransformTrack& transformTrack);
    
}; // TransformUtilities
