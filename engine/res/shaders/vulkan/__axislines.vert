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

layout(location = 0) in vec2 inPosition;
layout(location = 0) out vec3 outPosition;

void main()
{
    outPosition = vec3(inPosition.x * 1000 + CAMERA.position.x, 0, inPosition.y * 1000 + CAMERA.position.z);
    gl_Position = CAMERA.viewProj * vec4(outPosition, 1.0);
}