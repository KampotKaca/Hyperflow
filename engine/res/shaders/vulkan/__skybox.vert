#version 450

layout(set = 0, binding = 0) uniform Camera
{
    vec3 lookDirection;
    vec3 position;
    mat4 view;
    mat4 invView;
    mat4 proj;
    mat4 invProj;
    mat4 viewProj;
} CAMERA;

layout(location = 0) in vec3 inPosition;
layout(location = 0) out vec3 outTexcoord;

void main()
{
    vec4 position = CAMERA.viewProj * vec4(inPosition, 1.0);
    gl_Position = position.xyww;
    outTexcoord = inPosition;
}