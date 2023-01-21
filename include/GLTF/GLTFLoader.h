#pragma once

#include <string>
#include <vector>

class Clip;
struct cgltf_data;
class Pose;
class Skeleton;

class GLTFLoader
{
public:
    static cgltf_data* LoadGLTFFile(const char* path);
    static void FreeGLTFFile(cgltf_data* data);
    static Pose LoadRestPose(const cgltf_data* data);
    static Pose LoadBindPose(const cgltf_data* data);
    static std::vector<std::string> LoadJointNames(const cgltf_data* data);
    static Skeleton LoadSkeleton(const cgltf_data* data);
    static std::vector<Clip> LoadAnimationClips(const cgltf_data* data);
    
}; // GLTFLoader

