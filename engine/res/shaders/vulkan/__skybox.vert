#version 450
#include "global"

layout(location = 0) in vec3 inPosition;
layout(location = 0) out vec3 outTexcoord;

void main()
{
    vec4 pos = GLOBAL.CAMERA.proj * mat4(mat3(GLOBAL.CAMERA.view)) * vec4(inPosition, 1.0);
    gl_Position = vec4(pos.xy, pos.w * 0.9999, pos.w);
    outTexcoord = inPosition;
}