#pragma once

#include <string>
#include <vector>

struct Vec3;
struct Quat;
class Pose;
template<typename T, unsigned int N> class Track;
template <typename T, unsigned int N> class FastTrack;
template <typename VTRACK, typename QTRACK> class TTransformTrack;

template <typename TRACK>
class TClip
{
public:
    TClip();
    ~TClip();
    
    unsigned int GetSize() const;
    unsigned int GetIDAtIndex(unsigned int idx) const;
    const std::string& GetName() const;
    float GetStartTime() const;
    float GetEndTime() const;
    float GetDuration() const;
    bool IsLooping() const;

    void SetIDAtIndex(unsigned int idx, unsigned int id);
    void SetName(const std::string& name);
    void SetLooping(bool bLooping);
    // Sets start/end based on the owned tracks
    void RecalculateDuration();

    float Sample(Pose& outPose, float t) const;
    TRACK& operator[](unsigned int id);
    const TRACK& operator[] (unsigned int id) const;
    
protected:
    mutable std::vector<TRACK> m_Tracks;
    std::string m_Name;
    float m_StartTime;
    float m_EndTime;
    bool m_Looping;

    float AdjustTimeToFitRange(float t) const;
    
}; // Clip

typedef TClip<TTransformTrack<Track<Vec3, 3>, Track<Quat, 4>>> Clip;
typedef TClip<TTransformTrack<FastTrack<Vec3, 3>, FastTrack<Quat, 4>>> FastClip;

class ClipUtilities
{
public:
    static FastClip OptimizeClip(const Clip& clip);
    
}; // ClipUtilities