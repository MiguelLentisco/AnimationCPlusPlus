#include "Render/Shader.h"

#include "glad/glad.h"
#include <fstream>
#include <iostream>
#include <sstream>

// ---------------------------------------------------------------------------------------------------------------------

Shader::Shader()
{
    Handle = glCreateProgram();
    
} // Shader

// ---------------------------------------------------------------------------------------------------------------------

Shader::Shader(const std::string& vertex, const std::string& fragment) : Shader()
{
    Handle = glCreateProgram();
    Load(vertex, fragment);
    
} // Shader

// ---------------------------------------------------------------------------------------------------------------------

Shader::~Shader()
{
    glDeleteProgram(Handle);
    
} // ~Shader

// ---------------------------------------------------------------------------------------------------------------------

void Shader::Load(const std::string& vertex, const std::string& fragment)
{
    auto LoadShader = [](const std::string& shader) -> std::string
    {
        std::ifstream file(shader.c_str());
        const bool bShaderFile = file.good();
        file.close();

        return bShaderFile ? ReadFile(shader) : shader;
    };

    const std::string vertexSource = LoadShader(vertex);
    const std::string fragmentSource = LoadShader(fragment);

    const unsigned vertexID = CompileVertexShader(vertexSource);
    const unsigned int fragmentID = CompileFragmentShader(fragmentSource);

    if (!LinkShaders(vertexID, fragmentID))
    {
        return;
    }

    PopulateAttributes();
    PopulateUniforms();
    
} // Load

// ---------------------------------------------------------------------------------------------------------------------

void Shader::Bind()
{
    glUseProgram(Handle);
    
} // Bind

// ---------------------------------------------------------------------------------------------------------------------

void Shader::Unbind()
{
    glUseProgram(0);
    
} // Unbind

// ---------------------------------------------------------------------------------------------------------------------

unsigned Shader::GetHandle() const
{
    return Handle;
    
} // GetHandle

// ---------------------------------------------------------------------------------------------------------------------

unsigned Shader::GetAttribute(const std::string& name) const
{
    const auto it = Attributes.find(name);
    if (it == Attributes.end())
    {
        std::cout << "Bad attrib index: " << name << std::endl;
        return 0;
    }

    return it->second;
    
} // GetAttribute

// ---------------------------------------------------------------------------------------------------------------------

unsigned Shader::GetUniform(const std::string& name) const
{
    const auto it = Uniforms.find(name);
    if (it == Uniforms.end())
    {
        std::cout << "Bad uniform index: " << name << std::endl;
        return 0;
    }

    return it->second;
    
} // GetUniform

// ---------------------------------------------------------------------------------------------------------------------

std::string Shader::ReadFile(const std::string& path)
{
    std::ifstream file;
    file.open(path);

    std::stringstream contents;
    contents << file.rdbuf();
    file.close();

    return contents.str();
    
} // ReadFile

// ---------------------------------------------------------------------------------------------------------------------

unsigned Shader::CompileVertexShader(const std::string& vertex)
{
    // Create and compile shader
    const unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char* shaderPtr = vertex.c_str();
    glShaderSource(vertexShader, 1, &shaderPtr, nullptr);
    glCompileShader(vertexShader);

    // Check errors
    int success = 0;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        static constexpr unsigned int LOG_SIZE = 512;
        char infoLog[LOG_SIZE];
        glGetShaderInfoLog(vertexShader, LOG_SIZE, nullptr, infoLog);
        std::cout << "Vertex compilation failed.\n";
        std::cout << "\t" << infoLog << std::endl;
        
        glDeleteShader(vertexShader);
        return 0;
    }

    return vertexShader;
    
} // CompileVertexShader

// ---------------------------------------------------------------------------------------------------------------------

unsigned Shader::CompileFragmentShader(const std::string& fragment)
{
    // Create and compile shader
    const unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char* shaderPtr = fragment.c_str();
    glShaderSource(fragmentShader, 1, &shaderPtr, nullptr);
    glCompileShader(fragmentShader);

    // Check errors
    int success = 0;
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        static constexpr unsigned int LOG_SIZE = 512;
        char infoLog[LOG_SIZE];
        glGetShaderInfoLog(fragmentShader, LOG_SIZE, nullptr, infoLog);
        std::cout << "Fragment compilation failed.\n";
        std::cout << "\t" << infoLog << std::endl;
        
        glDeleteShader(fragmentShader);
        return 0;
    }

    return fragmentShader;
    
} // CompileFragmentShader

// ---------------------------------------------------------------------------------------------------------------------

bool Shader::LinkShaders(unsigned vertex, unsigned fragment) const
{
    glAttachShader(Handle, vertex);
    glAttachShader(Handle, fragment);
    glLinkProgram(Handle);

    int success = 0;
    glGetProgramiv(Handle, GL_LINK_STATUS, &success);
    if (!success)
    {
        static constexpr unsigned int LOG_SIZE = 512;
        char infoLog[LOG_SIZE];
        glGetProgramInfoLog(Handle, LOG_SIZE, nullptr, infoLog);

        std::cout << "ERROR: Shader linked failed.\n";
        std::cout << "\t" << infoLog << std::endl;
    }
    
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    return success;
    
} // LinkShaders

// ---------------------------------------------------------------------------------------------------------------------

void Shader::PopulateAttributes()
{
    int count = -1;
    glUseProgram(Handle);
    glGetProgramiv(Handle, GL_ACTIVE_ATTRIBUTES, &count);

    static constexpr unsigned int NAME_SIZE = 128;
    char name[NAME_SIZE];
    int length;
    int size;
    GLenum type;
    
    for (int i = 0; i < count; ++i)
    {
        memset(name, 0, sizeof(char) * NAME_SIZE);
        glGetActiveAttrib(Handle, static_cast<GLuint>(i), NAME_SIZE, &length, &size, &type, name);
        const int attribute = glGetAttribLocation(Handle, name);
        if (attribute < 0)
        {
            continue;
        }
        
        Attributes[name] = attribute;
    }

    glUseProgram(0);
    
} // PopulateAttributes

// ---------------------------------------------------------------------------------------------------------------------

void Shader::PopulateUniforms()
{
    int count = -1;
    glUseProgram(Handle);
    glGetProgramiv(Handle, GL_ACTIVE_UNIFORMS, &count);

    static constexpr unsigned int NAME_SIZE = 128;
    char name[NAME_SIZE];
    int length;
    int size;
    GLenum type;

    static constexpr unsigned int TEST_NAME_SIZE = 256;
    char testName[TEST_NAME_SIZE];
    
    for (int i = 0; i < count; ++i)
    {
        memset(name, 0, sizeof(char) * NAME_SIZE);
        glGetActiveUniform(Handle, static_cast<GLuint>(i), NAME_SIZE, &length, &size, &type, name);
        const int uniform = glGetUniformLocation(Handle, name);
        if (uniform < 0)
        {
            continue;
        }

        // If uniform is array retrieve each index
        std::string uniformName = name;
        const std::size_t found = uniformName.find('[');
        if (found != std::string::npos)
        {
            uniformName.erase(uniformName.begin() + found, uniformName.end());
            unsigned int uniformIdx = 0;
            while (true)
            {
                memset(testName, 0, sizeof(char) * TEST_NAME_SIZE);
                sprintf_s(testName, "%s[%d]", uniformName.c_str(), uniformIdx++);
                const int uniformLocation = glGetUniformLocation(Handle, testName);
                if (uniformLocation < 0)
                {
                    break;
                }

                Uniforms[testName] = uniformLocation;
            }
        }

        Uniforms[uniformName] = uniform;
    }

    glUseProgram(0);
    
} // PopulateUniforms

// ---------------------------------------------------------------------------------------------------------------------

