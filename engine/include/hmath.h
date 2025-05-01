#ifndef HMATH_H
#define HMATH_H

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <cinttypes>
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"

#include "glm/mat4x4.hpp"
#include "glm/common.hpp"
#include "glm/fwd.hpp"
#include "glm/gtx/transform.hpp"

namespace hf
{
    typedef glm::vec2 vec2;
    typedef glm::vec3 vec3;
    typedef glm::vec4 vec4;

    typedef glm::ivec2 ivec2;
    typedef glm::ivec3 ivec3;
    typedef glm::ivec4 ivec4;

    typedef glm::uvec2 uvec2;
    typedef glm::uvec3 uvec3;
    typedef glm::uvec4 uvec4;

    typedef glm::quat quaternion;

    typedef glm::mat2   mat2;
    typedef glm::mat2x3 mat2x3;
    typedef glm::mat2x4 mat2x4;
    typedef glm::mat3x2 mat3x2;
    typedef glm::mat3   mat3;
    typedef glm::mat3x4 mat3x4;
    typedef glm::mat4x2 mat4x2;
    typedef glm::mat4x3 mat4x3;
    typedef glm::mat4   mat4;
}

#endif //HMATH_H
