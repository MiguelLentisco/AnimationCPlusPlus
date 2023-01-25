#pragma once

struct Vec3;
struct Quat;
class Pose;
class Skeleton;
template <typename T, unsigned int N> class Track;
template <typename T, unsigned int N> class FastTrack;
template <typename VTRACK, typename QTRACK> class TTransformTrack;
typedef TTransformTrack<Track<Vec3, 3>, Track<Quat, 4>> TransformTrack;
typedef TTransformTrack<FastTrack<Vec3, 3>, FastTrack<Quat, 4>> FastTransformTrack;
template <typename TRACK> class TClip;
typedef TClip<TTransformTrack<Track<Vec3, 3>, Track<Quat, 4>>> Clip;
typedef TClip<TTransformTrack<FastTrack<Vec3, 3>, FastTrack<Quat, 4>>> FastClip;

class AnimationUtilities
{
public:
    AnimationUtilities() = delete;
    AnimationUtilities(const AnimationUtilities&) = delete;
    AnimationUtilities& operator=(const AnimationUtilities&) = delete;
    
    template <typename T, unsigned int N>
    static FastTrack<T, N> OptimizeTrack(const Track<T, N>& track);
    static FastTransformTrack OptimizeTransformTrack(const TransformTrack& transformTrack);
    static FastClip OptimizeClip(const Clip& clip);

    template <typename TRACK>
    static Pose MakeAdditivePose(const Skeleton& skeleton, const TClip<TRACK>& clip);
    
}; // AnimationUtilities
