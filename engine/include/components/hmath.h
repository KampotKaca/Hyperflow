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

namespace hf
{
    typedef glm::vec2 vec2;
    typedef glm::vec3 vec3;
    typedef glm::vec4 vec4;

    typedef glm::ivec2 ivec2;
    typedef glm::ivec3 ivec3;
    typedef glm::ivec4 ivec4;

    typedef glm::quat quaternion;
}

#endif //HMATH_H
