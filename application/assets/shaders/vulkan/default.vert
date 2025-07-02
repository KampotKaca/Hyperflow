#version 450
#include "global"

layout(push_constant) uniform PushConstants
{
    mat4 model;
    vec4 albedo;
    float smoothness;
} PUSH_CONSTANT;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec2 o_TexCoord;
layout(location = 1) out vec3 o_Color;
layout(location = 2) out vec3 o_Normal;
layout(location = 3) out vec3 o_Position;

void main()
{
    gl_Position = GLOBAL.CAMERA.viewProj * PUSH_CONSTANT.model * vec4(inPosition, 1.0);
    o_Color = vec3(PUSH_CONSTANT.albedo);
    o_TexCoord = inTexCoord;
    o_Normal = mat3(PUSH_CONSTANT.model) * inNormal;
    o_Position = vec3(gl_Position);
}