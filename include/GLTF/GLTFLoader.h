#pragma once

#include <string>
#include <vector>

struct cgltf_data;
struct cgltf_node;
struct cgltf_accessor;
struct cgltf_attribute;
struct cgltf_skin;
struct cgltf_animation_channel;
struct Transform;
class Pose;
class Skeleton;
class SkeletalMesh;
struct Vec3;
struct Quat;
template<typename T, unsigned int N> class Track;
template<typename VTRACK, typename QTRACK> class TTransformTrack;
template<typename TRACK> class TClip;
typedef TClip<TTransformTrack<Track<Vec3, 3>, Track<Quat, 4>>> Clip;
template<typename T, unsigned int N> class Track;

class GLTFLoader
{
public:
    static cgltf_data* LoadGLTFFile(const char* path);
    static void FreeGLTFFile(cgltf_data* data);
    static Pose LoadRestPose(const cgltf_data* data);
    static Pose LoadBindPose(const cgltf_data* data);
    static std::vector<std::string> LoadJointNames(const cgltf_data* data);
    static Skeleton LoadSkeleton(const cgltf_data* data);
    static std::vector<SkeletalMesh> LoadSkeletalMeshes(const cgltf_data* data);
    static std::vector<SkeletalMesh> LoadStaticMeshes(const cgltf_data* data);
    static std::vector<Clip> LoadAnimationClips(const cgltf_data* data);

private:
    static Transform GetLocalTransforms(const cgltf_node& n);
    static int GetNodeIndex(const cgltf_node* target, const cgltf_node* allNodes, unsigned int numNodes);
    static void GetScalarValues(std::vector<float>& out, unsigned int compCount, const cgltf_accessor& accessor);
    static void MeshFromAttribute(SkeletalMesh& outMesh, const cgltf_attribute& attribute, const cgltf_skin* skin,
        const cgltf_node* nodes, unsigned int nodeCount);
    static std::vector<SkeletalMesh> LoadMeshes(const cgltf_data* data, bool bMustHaveSkin);
    template<typename T, int N>
    static void TrackFromChannel(Track<T, N>& result, const cgltf_animation_channel& channel);
    
}; // GLTFLoader

