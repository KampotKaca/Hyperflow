#version 450
#include "global"
#include "phonglighting"

layout(push_constant) uniform PushConstants
{
    vec4 phongData; //xyz specular color, w smoothness
} PUSH_CONSTANT;

layout(location = 0) in vec2 o_TexCoord;
layout(location = 1) in vec3 o_Color;
layout(location = 2) in vec3 o_WNormal;
layout(location = 3) in vec3 o_FragPosition;

layout(set = 1, binding = 0) uniform sampler2D viking_room;

layout(location = 0) out vec4 outColor;

void main()
{
    vec4 color = texture(viking_room, o_TexCoord) * vec4(o_Color, 1.0f);
    PhongLightInfo lightInfo;
    lightInfo.ambient = 0.2f;
    lightInfo.smoothness = PUSH_CONSTANT.phongData.w;
    lightInfo.specularColor = PUSH_CONSTANT.phongData.xyz;
    outColor = vec4(GetPhongLighting(color.rgb, GLOBAL.CAMERA.position, o_FragPosition, o_WNormal, lightInfo), color.w);
}