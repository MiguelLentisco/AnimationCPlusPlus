#include "GLTF/GLTFLoader.h"

#include <iostream>
#include <ostream>

#include "Animation/Clip.h"
#include "Animation/Frame.h"
#include "Animation/Interpolation.h"
#include "Animation/Track.h"
#include "Animation/TransformTrack.h"
#include "Core/Mat4.h"
#include "Core/Transform.h"
#include "GLTF/cgltf.h"
#include "SkeletalMesh/Pose.h"
#include "SkeletalMesh/Skeleton.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace GLTFHelpers
{
    Transform GetLocalTransforms(const cgltf_node& n)
    {
        if (n.has_matrix)
        {
            return Transform::FromMat4(Mat4(&n.matrix[0]));
        }

        Transform result;
        if (n.has_translation)
        {
            result.position = {&n.translation[0]};
        }
        if (n.has_rotation)
        {
            result.rotation = {&n.rotation[0]};
        }
        if (n.has_scale)
        {
            result.scale =  {&n.scale[0]};
        }

        return result;
        
    } // GetLocalTransforms
    
    // -----------------------------------------------------------------------------------------------------------------

    int GetNodeIndex(const cgltf_node* target, const cgltf_node* allNodes, unsigned int numNodes)
    {
        if (target == nullptr)
        {
            return -1;
        }

        for (unsigned int i = 0; i < numNodes; ++i)
        {
            if (target == &allNodes[i])
            {
                return static_cast<int>(i);
            }
        }
        
        return -1;
        
    } // GetNodeIndex

    // -----------------------------------------------------------------------------------------------------------------

    void GetScalarValues(std::vector<float>& out, unsigned int compCount, const cgltf_accessor& accessor)
    {
        out.resize(accessor.count * compCount);
        for (cgltf_size i = 0; i < accessor.count; ++i)
        {
            cgltf_accessor_read_float(&accessor, i, &out[i * compCount], compCount);
        }
        
    } // GetScalarValues

    // -----------------------------------------------------------------------------------------------------------------

    template<typename T, int N>
    void TrackFromChannel(Track<T, N>& result, const cgltf_animation_channel& channel)
    {
        const cgltf_animation_sampler& sampler = *channel.sampler;

        // Set interpolation
        Interpolation interpolation = Interpolation::Constant;
        bool bSamplerCubic = false;
        if (sampler.interpolation == cgltf_interpolation_type_linear)
        {
            interpolation = Interpolation::Linear;
        }
        else if (sampler.interpolation == cgltf_interpolation_type_cubic_spline)
        {
            bSamplerCubic = true;
            interpolation = Interpolation::Cubic;
        }
        result.SetInterpolation(interpolation);

        // Get time and values and fill data
        std::vector<float> time;
        GetScalarValues(time, 1, *sampler.input);

        std::vector<float> val;
        GetScalarValues(val, N, *sampler.output);

        const unsigned int numFrames = sampler.input->count;
        const unsigned int compCount = val.size() / time.size();
        result.Resize(numFrames);

        for (unsigned int i = 0; i < numFrames; ++i)
        {
            const unsigned int baseIdx = i * compCount;
            unsigned int offset = 0;
            Frame<N>& frame = result[i];

            frame.m_Time = time[i];

            for (unsigned int comp = 0; comp < N; ++comp)
            {
                frame.m_In[comp] = bSamplerCubic ? val[baseIdx + offset++] : 0.f;
            }
            
            for (unsigned int comp = 0; comp < N; ++comp)
            {
                frame.m_Value[comp] = val[baseIdx + offset++];
            }
            
            for (unsigned int comp = 0; comp < N; ++comp)
            {
                frame.m_Out[comp] = bSamplerCubic ? val[baseIdx + offset++] : 0.f;
            }
        }
        
    } // TrackFromChannel
    
} // GLTFHelpers

// ---------------------------------------------------------------------------------------------------------------------

cgltf_data* GLTFLoader::LoadGLTFFile(const char* path)
{
    constexpr cgltf_options options = {};
    cgltf_data* data = nullptr;
    cgltf_result result = cgltf_parse_file(&options, path, &data);

    if (result != cgltf_result_success)
    {
        std::cout << "Could not load: " << path << std::endl;
        return nullptr;
    }

    result = cgltf_load_buffers(&options, data, path);
    if (result != cgltf_result_success)
    {
        cgltf_free(data);
        std::cout << "Could not load: " << path << std::endl;
        return nullptr;
    }

    result = cgltf_validate(data);
    if (result != cgltf_result_success)
    {
        cgltf_free(data);
        std::cout << "Invalid file: " << path << std::endl;
        return nullptr;
    }

    return data;
    
} // LoadGLTFFile

// ---------------------------------------------------------------------------------------------------------------------

void GLTFLoader::FreeGLTFFile(cgltf_data* data)
{
    if (data == nullptr)
    {
        std::cout << "WARNING: Can't free null data" << std::endl;
        return;
    }

    cgltf_free(data);
    
} // FreeGLTFFile

// ---------------------------------------------------------------------------------------------------------------------

Pose GLTFLoader::LoadRestPose(const cgltf_data* data)
{
    const unsigned int boneCount = data->nodes_count;
    Pose result(boneCount);

    for (unsigned int i = 0; i < boneCount; ++i)
    {
        cgltf_node& node = data->nodes[i];
        result.SetLocalTransform(i, GLTFHelpers::GetLocalTransforms(node));
        result.SetParent(i, GLTFHelpers::GetNodeIndex(node.parent, data->nodes, boneCount));
    }

    return result;
       
} // LoadRestPose

// ---------------------------------------------------------------------------------------------------------------------

Pose GLTFLoader::LoadBindPose(const cgltf_data* data)
{
    const Pose restPose = LoadRestPose(data);
    const unsigned int numBones = restPose.GetSize();

    // Use rest pose as default value if inverse bind pose is not available
    std::vector<Transform> worldBindPose(numBones);
    for (unsigned int i = 0; i < numBones; ++i)
    {
        worldBindPose[i] = restPose.GetGlobalTransform(i);
    }

    // Load and use inverse bind pose to obtain the bind transform
    const unsigned int numSkins = data->skins_count;
    for (unsigned int i = 0; i < numSkins; ++i)
    {
        const cgltf_skin& skin = data->skins[i];
        std::vector<float> invBindAccessor;
        GLTFHelpers::GetScalarValues(invBindAccessor, 16, *skin.inverse_bind_matrices);

        const unsigned int numJoints = skin.joints_count;
        for (unsigned int j = 0; j < numJoints; ++j)
        {
            const Transform bindTransform = Transform::FromMat4(Mat4(&invBindAccessor[j * 16]).Inverse());
            const int jointIdx = GLTFHelpers::GetNodeIndex(skin.joints[j], data->nodes, numBones);
            worldBindPose[jointIdx] = bindTransform;
        }
    }

    // Obtain the local transform of each joint using the world transform of the parent joint
    Pose bindPose = restPose;
    for (unsigned int i = 0; i < numBones; ++i)
    {
        Transform current = worldBindPose[i];
        
        const int parentID = bindPose.GetParent(i);
        if (parentID >= 0)
        {
            const Transform& parent = worldBindPose[parentID];
            current = parent.Inverse().Combine(current);
        }

        bindPose.SetLocalTransform(i, current);
    }

    return bindPose;
    
} // LoadBindPose

// ---------------------------------------------------------------------------------------------------------------------

std::vector<std::string> GLTFLoader::LoadJointNames(const cgltf_data* data)
{
    const unsigned int boneCount = data->nodes_count;
    std::vector<std::string> result(boneCount, "Not Set");

    for (unsigned int i = 0; i < boneCount; ++i)
    {
        const cgltf_node& node = data->nodes[i];
        result[i] = node.name == nullptr ? "EMPTY NODE" : node.name;
    }

    return result;
    
} // LoadJointNames

// ---------------------------------------------------------------------------------------------------------------------

Skeleton GLTFLoader::LoadSkeleton(const cgltf_data* data)
{
    return {LoadRestPose(data), LoadBindPose(data), LoadJointNames(data)};
    
} // LoadSkeleton

// ---------------------------------------------------------------------------------------------------------------------

std::vector<Clip> GLTFLoader::LoadAnimationClips(const cgltf_data* data)
{
    const unsigned int numClips = data->animations_count;
    const unsigned int numNodes = data->nodes_count;

    std::vector<Clip> result;
    result.resize(numClips);

    for (unsigned int i = 0; i < numClips; ++i)
    {
        const cgltf_animation& animation = data->animations[i];
        result[i].SetName(animation.name);

        const unsigned int numChannels = animation.channels_count;
        for (unsigned int j = 0; j < numChannels; ++j)
        {
            const cgltf_animation_channel& channel = animation.channels[j];
            const cgltf_node* target = channel.target_node;
            const int nodeID = GLTFHelpers::GetNodeIndex(target, data->nodes, numNodes);

            TransformTrack& transformTrack = result[i][nodeID];
            if (channel.target_path == cgltf_animation_path_type_translation)
            {
                VectorTrack& track = transformTrack.GetPositionTrack();
                GLTFHelpers::TrackFromChannel<Vec3, 3>(track, channel);
            }
            else if (channel.target_path == cgltf_animation_path_type_scale)
            {
                VectorTrack& track = transformTrack.GetScaleTrack();
                GLTFHelpers::TrackFromChannel<Vec3, 3>(track, channel);
            }
            else if (channel.target_path == cgltf_animation_path_type_rotation)
            {
                QuaternionTrack& track = transformTrack.GetRotationTrack();
                GLTFHelpers::TrackFromChannel<Quat,4>(track, channel);
            }
        }

        result[i].RecalculateDuration();
    }

    return result;
    
} // LoadAnimationClips

// ---------------------------------------------------------------------------------------------------------------------

