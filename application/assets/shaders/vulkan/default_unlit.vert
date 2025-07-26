#version 450
#include "global"

layout(push_constant) uniform PushConstants
{
    mat4 model;
    vec4 albedo;
} PUSH_CONSTANT;

layout(location = 0) in vec3 inPosition;
layout(location = 0) out vec3 o_Color;

void main()
{
    gl_Position = GLOBAL.CAMERA.viewProj * PUSH_CONSTANT.model * vec4(inPosition, 1.0);
    o_Color = vec3(PUSH_CONSTANT.albedo);
}