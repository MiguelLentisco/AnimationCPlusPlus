#include "Render/AnimTexture.h"

#include <cstring>
#include <fstream>
#include <iostream>

#include "Core/Quat.h"
#include "Core/TVec4.h"
#include "Core/Vec3.h"
#include "glad/glad.h"

// ---------------------------------------------------------------------------------------------------------------------

AnimTexture::AnimTexture()
{
    glGenTextures(1, &m_Handle);
    
} // AnimTexture

// ---------------------------------------------------------------------------------------------------------------------

AnimTexture::AnimTexture(const AnimTexture& other) : AnimTexture()
{
    *this = other;
    
} // AnimTexture

// ---------------------------------------------------------------------------------------------------------------------

AnimTexture& AnimTexture::operator=(const AnimTexture& other)
{
    if (this == &other)
    {
        return *this;
    }

    delete[] m_Data;
    m_Data = nullptr;
    m_Size = other.m_Size;

    if (m_Size == 0)
    {
        return *this;
    }

    const unsigned int dataSize = m_Size * m_Size * 4;
    m_Data = new float[dataSize];
    memcpy(m_Data, other.m_Data, sizeof(float) * dataSize);

    return *this;
    
} // operator=

// ---------------------------------------------------------------------------------------------------------------------

AnimTexture::~AnimTexture()
{
    delete[] m_Data;
    glDeleteTextures(1, &m_Handle);
    
} // ~AnimTexture

// ---------------------------------------------------------------------------------------------------------------------

void AnimTexture::Resize(unsigned int newSize)
{
    delete[] m_Data;

    m_Size = newSize;
    if (m_Size == 0)
    {
        return;
    }

    m_Data = new float[m_Size * m_Size * 4];
    
} // Resize

// ---------------------------------------------------------------------------------------------------------------------

void AnimTexture::Load(const char* path)
{
    std::ifstream file;
    file.open(path, std::ios::in | std::ios::binary);
    if (!file.is_open())
    {
        std::cout << "Couldn't open " << path << std::endl;
        return;
    }

    file >> m_Size;
    if (m_Size == 0)
    {
        return;
    }

    const unsigned int dataSize = m_Size * m_Size * 4;
    m_Data = new float[dataSize];
    file.read(reinterpret_cast<char*>(m_Data), sizeof(float) * dataSize);
    file.close();

    UploadTextureDataToGPU();
    
} // Load

// ---------------------------------------------------------------------------------------------------------------------

void AnimTexture::Save(const char* path) const
{
    std::ofstream file;
    file.open(path, std::ios::out | std::ios::binary);
    if (!file.is_open())
    {
        std::cout << "Couldn't open " << path << std::endl;
        return;
    }

    file << m_Size;
    if (m_Size == 0)
    {
        return;
    }

    file.write(reinterpret_cast<char*>(m_Data), sizeof(float) * m_Size * m_Size * 4);
    file.close();
    
} // Save

// ---------------------------------------------------------------------------------------------------------------------

void AnimTexture::UploadTextureDataToGPU()
{
    glBindTexture(GL_TEXTURE_2D, m_Handle);

    const auto size = static_cast<GLsizei>(m_Size);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, size, size, 0, GL_RGBA, GL_FLOAT, m_Data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);
    
} // UploadTextureDataToGPU

// ---------------------------------------------------------------------------------------------------------------------

void AnimTexture::SetTexel(unsigned x, unsigned y, const Vec3& v)
{
    const unsigned int idx = (y * m_Size * 4) + (x * 4);

    m_Data[idx + 0] = v.x;
    m_Data[idx + 1] = v.y;
    m_Data[idx + 2] = v.z;
    m_Data[idx + 3] = 0.f;
    
} // SetTexel

// ---------------------------------------------------------------------------------------------------------------------

void AnimTexture::SetTexel(unsigned x, unsigned y, const Quat& q)
{
    const unsigned int idx = (y * m_Size * 4) + (x * 4);

    m_Data[idx + 0] = q.x;
    m_Data[idx + 1] = q.y;
    m_Data[idx + 2] = q.z;
    m_Data[idx + 3] = q.w;
    
} // SetTexel

// ---------------------------------------------------------------------------------------------------------------------

Vec4 AnimTexture::GetTexel(unsigned x, unsigned y) const
{
    const unsigned int idx = (y * m_Size * 4) + (x * 4);

    return {m_Data[idx + 0], m_Data[idx + 1], m_Data[idx + 2], m_Data[idx + 3]};
    
} // GetTexel

// ---------------------------------------------------------------------------------------------------------------------

void AnimTexture::Set(unsigned uniformIdx, unsigned textureIdx)
{
    glActiveTexture(GL_TEXTURE0 + textureIdx);
    glBindTexture(GL_TEXTURE_2D, m_Handle);
    glUniform1i(static_cast<GLint>(uniformIdx), static_cast<GLint>(textureIdx));

} // Set

// ---------------------------------------------------------------------------------------------------------------------

void AnimTexture::Unset(unsigned textureIdx)
{
    glActiveTexture(GL_TEXTURE0 + textureIdx);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);
    
} // Unset

// ---------------------------------------------------------------------------------------------------------------------
