#version 450
#include "global"

layout(push_constant) uniform PushConstants
{
    mat4 model;
    vec4 albedo;
    float smoothness;
} PUSH_CONSTANT;

layout(location = 0) in vec2 o_TexCoord;
layout(location = 1) in vec3 o_Color;
layout(location = 2) in vec3 o_Normal;
layout(location = 3) in vec3 o_Position;

layout(set = 1, binding = 0) uniform sampler2D viking_room;

layout(location = 0) out vec4 outColor;

void main()
{
    outColor = texture(viking_room, o_TexCoord) * vec4(GetPhongLighting(o_Position, o_Normal, PUSH_CONSTANT.smoothness), 1.0f) * vec4(o_Color, 1.0f);
}