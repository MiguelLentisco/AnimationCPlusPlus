#pragma once

struct Quat;
struct Vec3;
template <typename T> struct TVec4;
typedef TVec4<float> Vec4;

class AnimTexture
{
public:
    AnimTexture();
    AnimTexture(const AnimTexture& other);
    AnimTexture& operator=(const AnimTexture& other);
    ~AnimTexture();

    const float* GetData() const { return m_Data; }
    unsigned int GetSize() const { return m_Size; }
    unsigned int GetHandle() const { return m_Handle; }

    void Resize(unsigned int newSize);

    void Load(const char* path);
    void Save(const char* path) const;
    void UploadTextureDataToGPU();

    void SetTexel(unsigned int x, unsigned int y, const Vec3& v);
    void SetTexel(unsigned int x, unsigned int y, const Quat& q);
    Vec4 GetTexel(unsigned int x, unsigned int y) const;

    void Set(unsigned int uniformIdx, unsigned int textureIdx);
    void Unset(unsigned int textureIdx);
    
protected:
    float* m_Data = nullptr;
    unsigned int m_Size = 0;
    unsigned int m_Handle = 0;
    
}; // AnimTexture
