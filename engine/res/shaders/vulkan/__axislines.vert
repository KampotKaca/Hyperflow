#version 450
#include "global"

layout(location = 0) in vec2 inPosition;
layout(location = 0) out vec3 outPosition;

void main()
{
    outPosition = vec3(inPosition.x * 1000 + GLOBAL.CAMERA.position.x, 0, inPosition.y * 1000 + GLOBAL.CAMERA.position.z);
    gl_Position = GLOBAL.CAMERA.viewProj * vec4(outPosition, 1.0);
}