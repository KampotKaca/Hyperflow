#version 450
#include "global"

layout(location = 0) in vec2 inPosition;
layout(location = 0) out vec3 outPosition;

layout(push_constant) uniform PushConstants
{
    vec2 drawData;
    vec4 color;
} PUSH_CONSTANT;

void main()
{
    outPosition.x = inPosition.x * PUSH_CONSTANT.drawData.y + GLOBAL.CAMERA.position.x;
    outPosition.y = 0;
    outPosition.z = inPosition.y * PUSH_CONSTANT.drawData.y + GLOBAL.CAMERA.position.z;
    gl_Position = GLOBAL.CAMERA.viewProj * vec4(outPosition, 1.0);
}