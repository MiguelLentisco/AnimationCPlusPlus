#pragma once

class Texture
{
public:
    Texture();
    Texture(const char* path);
    ~Texture();

    Texture(const Texture& other) = delete;
    Texture& operator=(const Texture& other) = delete;

    unsigned int GetHandle() const;
    
    void Load(const char* path);
    void Set(unsigned int uniformIdx, unsigned int textureIdx);
    void Unset(unsigned int textureIdx);

protected:
    unsigned int Width;
    unsigned int Height;
    unsigned int Channels;
    unsigned int Handle;
    
}; // Texture
