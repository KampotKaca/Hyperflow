#version 450

layout(binding = 0) uniform Camera
{
    mat4 model;
    mat4 view;
    mat4 proj;
    mat4 viewProj;
} CAMERA;

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 o_FragColor;

void main()
{
    gl_Position = CAMERA.viewProj * CAMERA.model * vec4(inPosition, 0.0, 1.0);
    o_FragColor = inColor;
}