#include "Render/Texture.h"

#include "glad/glad.h"
#include "Image/stb_image.h"

// ---------------------------------------------------------------------------------------------------------------------

Texture::Texture() : Width(0), Height(0), Channels(0)
{
    glGenTextures(1, &Handle);
    
} // Texture

// ---------------------------------------------------------------------------------------------------------------------

Texture::Texture(const char* path) : Texture()
{
    Load(path);
    
} // Texture

// ---------------------------------------------------------------------------------------------------------------------

Texture::~Texture()
{
    glDeleteTextures(1, &Handle);
    
} // Texture

// ---------------------------------------------------------------------------------------------------------------------

unsigned int Texture::GetHandle() const
{
    return Handle;
    
} // GetHandle

// ---------------------------------------------------------------------------------------------------------------------

void Texture::Load(const char* path)
{
    glBindTexture(GL_TEXTURE_2D, Handle);

    // Load image to GPU
    int width, height, channels;
    unsigned char* data = stbi_load(path, &width, &height, &channels, 4);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    // Minmaps and wrap mode
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);

    Width = width;
    Height = height;
    Channels = channels;
    
} // Load

// ---------------------------------------------------------------------------------------------------------------------

void Texture::Set(unsigned uniformIdx, unsigned textureIdx)
{
    glActiveTexture(GL_TEXTURE0 + textureIdx);
    glBindTexture(GL_TEXTURE_2D, Handle);
    glUniform1i(static_cast<GLint>(uniformIdx), static_cast<GLint>(textureIdx));
    
} // Set

// ---------------------------------------------------------------------------------------------------------------------

void Texture::Unset(unsigned textureIdx)
{
    glActiveTexture(GL_TEXTURE0 + textureIdx);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);
    
}

// ---------------------------------------------------------------------------------------------------------------------
