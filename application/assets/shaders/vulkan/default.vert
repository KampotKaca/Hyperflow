#version 450
#include "global"

layout(push_constant) uniform PushConstants
{
    mat4 model;
} PUSH_CONSTANT;

//struct Material
//{
//
//};
//
//layout(std430, binding = 0) buffer MaterialBuffer
//{
//    uint data[];
//};

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec2 o_TexCoord;
layout(location = 1) out vec3 o_Color;

void main()
{
    gl_Position = GLOBAL.CAMERA.viewProj * PUSH_CONSTANT.model * vec4(inPosition, 1.0);
    o_Color = inColor;
    o_Color.r *= (sin(float(GLOBAL.TIME.timeSinceStartup)) + 1) * 0.5f;
    o_TexCoord = inTexCoord;
}