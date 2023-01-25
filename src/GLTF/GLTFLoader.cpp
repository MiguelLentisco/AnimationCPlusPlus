#include "GLTF/GLTFLoader.h"

#include <iostream>
#include <ostream>

#include "Animation/Clip.h"
#include "Animation/Frame.h"
#include "Animation/Interpolation.h"
#include "Animation/Track.h"
#include "Animation/TransformTrack.h"
#include "Core/BasicUtils.h"
#include "Core/Mat4.h"
#include "Core/Transform.h"
#include "Core/TVec2.h"
#include "GLTF/cgltf.h"
#include "SkeletalMesh/Pose.h"
#include "SkeletalMesh/SkeletalMesh.h"
#include "SkeletalMesh/Skeleton.h"

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
        result.SetLocalTransform(i, GetLocalTransforms(node));
        result.SetParent(i, GetNodeIndex(node.parent, data->nodes, boneCount));
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
        GetScalarValues(invBindAccessor, 16, *skin.inverse_bind_matrices);

        const unsigned int numJoints = skin.joints_count;
        for (unsigned int j = 0; j < numJoints; ++j)
        {
            const Transform bindTransform = Transform::FromMat4(Mat4(&invBindAccessor[j * 16]).Inverse());
            const int jointIdx = GetNodeIndex(skin.joints[j], data->nodes, numBones);
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

std::vector<SkeletalMesh> GLTFLoader::LoadMeshes(const cgltf_data* data)
{
    std::vector<SkeletalMesh> result;
    const cgltf_node* nodes = data->nodes;
    const unsigned int nodeCount = data->nodes_count;

    for (unsigned int i = 0; i < nodeCount; ++i)
    {
        const cgltf_node& node = nodes[i];
        if (node.mesh == nullptr || node.skin == nullptr)
        {
            continue;
        }

        const unsigned int numPrimitives = node.mesh->primitives_count;
        for (unsigned int j = 0; j < numPrimitives; ++j)
        {
            result.emplace_back();
            SkeletalMesh& skeletalMesh = result[result.size() - 1];
            
            const cgltf_primitive& primitive = node.mesh->primitives[j];
            const unsigned int attributesCount = primitive.attributes_count;
            for (unsigned int k = 0; k < attributesCount; ++k)
            {
                const cgltf_attribute& attribute = primitive.attributes[k];
                MeshFromAttribute(skeletalMesh, attribute, node.skin, nodes, nodeCount);
            }

            if (primitive.indices != nullptr)
            {
                const unsigned int indicesCount = primitive.indices->count;
                std::vector<unsigned int>& indices = skeletalMesh.GetIndices();
                indices.resize(indicesCount);
            
                for (unsigned int k = 0; k < indicesCount; ++k)
                {
                    indices[k] = cgltf_accessor_read_index(primitive.indices, k);
                } 
            }

            skeletalMesh.UpdateOpenGLBuffers();
        }
    }

    return result;
    
} // LoadMeshes

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
            const int nodeID = GetNodeIndex(target, data->nodes, numNodes);

            TransformTrack& transformTrack = result[i][nodeID];
            if (channel.target_path == cgltf_animation_path_type_translation)
            {
                VectorTrack& track = transformTrack.GetPositionTrack();
                TrackFromChannel<Vec3, 3>(track, channel);
            }
            else if (channel.target_path == cgltf_animation_path_type_scale)
            {
                VectorTrack& track = transformTrack.GetScaleTrack();
                TrackFromChannel<Vec3, 3>(track, channel);
            }
            else if (channel.target_path == cgltf_animation_path_type_rotation)
            {
                QuaternionTrack& track = transformTrack.GetRotationTrack();
                TrackFromChannel<Quat,4>(track, channel);
            }
        }

        result[i].RecalculateDuration();
    }

    return result;
    
} // LoadAnimationClips

// ---------------------------------------------------------------------------------------------------------------------

Transform GLTFLoader::GetLocalTransforms(const cgltf_node& n)
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
    
// ---------------------------------------------------------------------------------------------------------------------

int GLTFLoader::GetNodeIndex(const cgltf_node* target, const cgltf_node* allNodes, unsigned int numNodes)
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

// ---------------------------------------------------------------------------------------------------------------------

void GLTFLoader::GetScalarValues(std::vector<float>& out, unsigned int compCount, const cgltf_accessor& accessor)
{
    out.resize(accessor.count * compCount);
    for (cgltf_size i = 0; i < accessor.count; ++i)
    {
        cgltf_accessor_read_float(&accessor, i, &out[i * compCount], compCount);
    }
    
} // GetScalarValues

// ---------------------------------------------------------------------------------------------------------------------

void GLTFLoader::MeshFromAttribute(SkeletalMesh& outMesh, const cgltf_attribute& attribute, const cgltf_skin* skin,
    const cgltf_node* nodes, unsigned nodeCount)
{
    const cgltf_accessor& accessor = *attribute.data;

    // Find how many attributes has
    unsigned int componentCount;
    switch (accessor.type)
    {
        case cgltf_type_vec2:
            componentCount = 2;
        break;
        case cgltf_type_vec3:
            componentCount = 3;
            break;
        case cgltf_type_vec4:
            componentCount = 4;
            break;
        default:
            componentCount = 0;
    }

    // Load values
    std::vector<float> values;
    GetScalarValues(values, componentCount, accessor);

    // Fill appropriate data with the attribute data
    const unsigned int accessorCount = accessor.count;
    for (unsigned int i = 0; i < accessorCount; ++i)
    {
        const float* attributeValue = &values[i * componentCount];
        switch (attribute.type)
        {
            case cgltf_attribute_type_position:
                outMesh.GetPosition().emplace_back(Vec3{attributeValue});
                break;
            case cgltf_attribute_type_texcoord:
                outMesh.GetTexCoord().emplace_back(Vec2{attributeValue});
                break;
            case cgltf_attribute_type_weights:
                outMesh.GetBonesWeight().emplace_back(Vec4{attributeValue});
                break;
            case cgltf_attribute_type_normal:
            {
                const Vec3 normal = Vec3{attributeValue}.Normalized();
                outMesh.GetNormal().emplace_back(normal.IsZeroVec() ? Vec3{0.f, 1.f, 0.f} : normal);
                break;  
            }
            case cgltf_attribute_type_joints:
            {
                IVec4 bonesID;
                // Replace node ID to skeleton hierarchy boneID
                for (unsigned int j = 0; j < 4; ++j)
                {
                    const int nodeID = BasicUtils::FloatToInt(*(attributeValue + j));
                    const int boneID = GetNodeIndex(skin->joints[nodeID], nodes, nodeCount);
                    bonesID[j] = std::max(0, boneID);
                }
                outMesh.GetBonesID().emplace_back(bonesID);
                break;
            }
            default:
                break;
        }
    }
    
} // MeshFromAttribute

// ---------------------------------------------------------------------------------------------------------------------

template<typename T, int N>
void GLTFLoader::TrackFromChannel(Track<T, N>& result, const cgltf_animation_channel& channel)
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

// ---------------------------------------------------------------------------------------------------------------------
