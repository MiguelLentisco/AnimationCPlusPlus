#include "Render/Uniform.h"

#include <vector>

#include "Core/Mat4.h"
#include "Core/Quat.h"
#include "Core/TVec2.h"
#include "Core/TVec4.h"
#include "Core/Vec3.h"
#include "glad/glad.h"

// ---------------------------------------------------------------------------------------------------------------------

template Uniform<int>;
template Uniform<float>;
template Uniform<Vec2>;
template Uniform<Vec3>;
template Uniform<Vec4>;
template Uniform<IVec4>;
template Uniform<Mat4>;

// ---------------------------------------------------------------------------------------------------------------------

#define UNIFORM_IMPL(gl_func, tType, dType) \
    void Uniform<tType>::Set(unsigned int slot, const tType* data, unsigned int length) \
        { gl_func(static_cast<GLint>(slot), static_cast<GLsizei>(length), reinterpret_cast<const dType*>(data)); }

UNIFORM_IMPL(glUniform2iv, int, int)
UNIFORM_IMPL(glUniform2iv, IVec2, int)
UNIFORM_IMPL(glUniform4iv, IVec4, int)
UNIFORM_IMPL(glUniform1fv, float, float)
UNIFORM_IMPL(glUniform2fv, Vec2, float)
UNIFORM_IMPL(glUniform3fv, Vec3, float)
UNIFORM_IMPL(glUniform4fv, Vec4, float)
UNIFORM_IMPL(glUniform4fv, Quat, float)

template<>
void Uniform<Mat4>::Set(unsigned int slot, const Mat4* inputArray, unsigned int arrayLength)
{
    glUniformMatrix4fv(static_cast<GLint>(slot), static_cast<GLsizei>(arrayLength), false,
        reinterpret_cast<const float*>(inputArray));
    
} // Set

// ---------------------------------------------------------------------------------------------------------------------

template <typename T>
void Uniform<T>::Set(unsigned slot, const T& value)
{
    Set(slot, &value, 1);
    
} // Set

// ---------------------------------------------------------------------------------------------------------------------

template <typename T>
void Uniform<T>::Set(unsigned slot, const std::vector<T>& arr)
{
    Set(slot, &arr[0], arr.size());
    
} // Set

// ---------------------------------------------------------------------------------------------------------------------
