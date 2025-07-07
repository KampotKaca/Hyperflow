#ifndef HMATH_H
#define HMATH_H

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_RIGHT_HANDED
#define GLM_ENABLE_EXPERIMENTAL
#include <cinttypes>
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"

#include "glm/mat4x4.hpp"
#include "glm/common.hpp"
#include "glm/fwd.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/detail/type_quat.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtx/rotate_vector.hpp"

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

    typedef glm::quat quat;

    typedef glm::mat2   mat2;
    typedef glm::mat2x3 mat2x3;
    typedef glm::mat2x4 mat2x4;
    typedef glm::mat3x2 mat3x2;
    typedef glm::mat3   mat3;
    typedef glm::mat3x4 mat3x4;
    typedef glm::mat4x2 mat4x2;
    typedef glm::mat4x3 mat4x3;
    typedef glm::mat4   mat4;

    struct Rect
    {
        vec2 position{};
        vec2 size{};
    };

    struct IRect
    {
        ivec2 position{};
        ivec2 size{};
    };
}

#endif //HMATH_H
