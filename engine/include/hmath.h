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
#include "glm/gtx/euler_angles.hpp"

namespace hf
{
    typedef glm::i8vec2 i8vec2;
    typedef glm::i8vec3 i8vec3;
    typedef glm::i8vec4 i8vec4;

    typedef glm::u8vec2 u8vec2;
    typedef glm::u8vec3 u8vec3;
    typedef glm::u8vec4 u8vec4;

    typedef glm::i16vec2 i16vec2;
    typedef glm::i16vec3 i16vec3;
    typedef glm::i16vec4 i16vec4;

    typedef glm::u16vec2 u16vec2;
    typedef glm::u16vec3 u16vec3;
    typedef glm::u16vec4 u16vec4;

    typedef glm::ivec2 ivec2;
    typedef glm::ivec3 ivec3;
    typedef glm::ivec4 ivec4;

    typedef glm::uvec2 uvec2;
    typedef glm::uvec3 uvec3;
    typedef glm::uvec4 uvec4;

    typedef glm::i64vec2 i64vec2;
    typedef glm::i64vec3 i64vec3;
    typedef glm::i64vec4 i64vec4;

    typedef glm::u64vec2 u64vec2;
    typedef glm::u64vec3 u64vec3;
    typedef glm::u64vec4 u64vec4;

    typedef glm::vec2 vec2;
    typedef glm::vec3 vec3;
    typedef glm::vec4 vec4;

    typedef glm::dvec2 dvec2;
    typedef glm::dvec3 dvec3;
    typedef glm::dvec4 dvec4;

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
