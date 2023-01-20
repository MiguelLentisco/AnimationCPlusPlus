#pragma once

#include <map>
#include <string>

class Shader
{
public:
    Shader();
    Shader(const std::string& vertex, const std::string& fragment);
    ~Shader();
    
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    void Load(const std::string& vertex, const std::string& fragment);
    void Bind();
    void Unbind();

    unsigned int GetHandle() const;
    unsigned int GetAttribute(const std::string& name) const;
    unsigned int GetUniform(const std::string& name) const;
    
private:
    unsigned int Handle;
    std::map<std::string, unsigned int> Attributes;
    std::map<std::string, unsigned int> Uniforms;

    static std::string ReadFile(const std::string& path);
    static unsigned int CompileVertexShader(const std::string& vertex);
    static unsigned int CompileFragmentShader(const std::string& fragment);
    bool LinkShaders(unsigned int vertex, unsigned int fragment) const;

    void PopulateAttributes();
    void PopulateUniforms();
    
}; // Shader
