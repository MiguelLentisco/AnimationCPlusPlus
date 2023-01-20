#pragma once

#include <string>
#include <vector>

class Pose;
class TransformTrack;

class Clip
{
public:
    Clip();
    ~Clip();
    
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
    TransformTrack& operator[](unsigned int id);
    
protected:
    std::vector<TransformTrack> m_Tracks;
    std::string m_Name;
    float m_StartTime;
    float m_EndTime;
    bool m_Looping;

    float AdjustTimeToFitRange(float t) const;
    
}; // Clip
